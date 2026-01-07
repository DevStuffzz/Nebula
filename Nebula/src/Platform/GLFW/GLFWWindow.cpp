#include "nbpch.h"
#include "GLFWWindow.h"

#include "Nebula/Events/ApplicationEvent.h"
#include "Nebula/Events/MouseEvent.h"
#include "Nebula/Events/KeyEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

#include <stb_image.h>

namespace Nebula {

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description) {
		NB_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowProps& props) {
		return new GLFWWindow(props);
	}

	GLFWWindow::GLFWWindow(const WindowProps& props)
	{
		Init(props);
	}

	GLFWWindow::~GLFWWindow()
	{
		Shutdown();
	}

	void GLFWWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.Fullscreen = props.Fullscreen;
		NB_CORE_INFO("Creating Window {0}: {1}, {2}", props.Title, props.Width, props.Height);



		if (!s_GLFWInitialized) {
			int success = glfwInit();
			NEB_ASSERT(success, "Could not initialize GLFW!");
			
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		GLFWmonitor* monitor = nullptr;
		if (props.Fullscreen)
		{
			monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		}

		m_Window = glfwCreateWindow(
			(int)props.Width,
			(int)props.Height,
			m_Data.Title.c_str(),
			monitor,
			nullptr
		);

		// Load and set window icon
		// Note: Title bar icons are limited to 16x16 by Windows, but taskbar/Alt+Tab use larger sizes
		stbi_set_flip_vertically_on_load(0); // Don't flip icon
		int iconWidth, iconHeight, iconChannels;
		unsigned char* iconPixels = stbi_load("Library/logo.png", &iconWidth, &iconHeight, &iconChannels, 4);
		if (iconPixels)
		{
			GLFWimage icon;
			icon.width = iconWidth;
			icon.height = iconHeight;
			icon.pixels = iconPixels;
			glfwSetWindowIcon(m_Window, 1, &icon);
			stbi_image_free(iconPixels);
			NB_CORE_INFO("Window icon set ({0}x{1}) - Title bar shows 16x16, taskbar shows larger", iconWidth, iconHeight);
		}
		else
		{
			NB_CORE_WARN("Failed to load window icon from Library/logo.png");
		}
		
		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);


		SetVSync(true);

		// Set GLFW Callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS: {
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT: {
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS: {
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void GLFWWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void GLFWWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool GLFWWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	bool GLFWWindow::ToggleFullscreen()
	{

		m_Data.Fullscreen = !m_Data.Fullscreen;
		SetFullscreen(m_Data.Fullscreen);
		return m_Data.Fullscreen;
	}

	void GLFWWindow::SetFullscreen(bool fullscreen)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		if (fullscreen)
		{
			glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_FALSE);

			glfwSetWindowMonitor(
				m_Window,
				monitor,
				0,
				0,
				mode->width,
				mode->height,
				mode->refreshRate
			);
		}
		else
		{
			int w = mode->width / 2;
			int h = mode->height / 2;
			int x = (mode->width - w) / 2;
			int y = (mode->height - h) / 2;

			glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_TRUE);

			glfwSetWindowMonitor(
				m_Window,
				nullptr,
				x,
				y,
				w,
				h,
				0
			);
		}

		m_Data.Fullscreen = fullscreen;
	}



	void GLFWWindow::SetCursorMode(bool disabled)
	{
		if (disabled)
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void GLFWWindow::SetCursorLockMode(int mode)
	{
		switch (mode)
		{
			case 0: // None - free cursor
				glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				break;
			case 1: // Locked - hidden and locked to center
				glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				break;
			case 2: // Confined - constrained to window
				glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				// Note: GLFW doesn't have direct window confinement
				break;
		}
	}

	void GLFWWindow::SetCursorVisible(bool visible)
	{
		// Only apply visibility if cursor is not locked (DISABLED mode hides it automatically)
		int currentMode = glfwGetInputMode(m_Window, GLFW_CURSOR);
		if (currentMode != GLFW_CURSOR_DISABLED)
		{
			if (visible)
				glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			else
				glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
	}

	int GLFWWindow::GetCursorLockMode() const
	{
		int currentMode = glfwGetInputMode(m_Window, GLFW_CURSOR);
		
		// Map GLFW cursor mode to our enum
		if (currentMode == GLFW_CURSOR_DISABLED)
			return 1; // Locked
		else if (currentMode == GLFW_CURSOR_NORMAL)
			return 0; // None/Free
		else
			return 0; // Default to free
	}

	bool GLFWWindow::GetCursorVisible() const
	{
		int currentMode = glfwGetInputMode(m_Window, GLFW_CURSOR);
		return (currentMode != GLFW_CURSOR_HIDDEN && currentMode != GLFW_CURSOR_DISABLED);
	}
	
	void GLFWWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}
}
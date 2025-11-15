#include "nbpch.h"
#include "OpenGLContext.h"
#include "Nebula/Core.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Nebula {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{

	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		NEB_CORE_ASSERT(status, "Failed To Initialize Glad");

		NB_CORE_INFO("OpenGL Info:");
		NB_CORE_INFO(" Vendor: {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		NB_CORE_INFO(" Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		NB_CORE_INFO(" Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

	}

	void OpenGLContext::SwapBuffers()
	{

		glfwSwapBuffers(m_WindowHandle);
	}
}

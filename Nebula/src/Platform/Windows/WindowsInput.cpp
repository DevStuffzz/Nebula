#include "nbpch.h"
#include "WindowsInput.h"


#include <GLFW/glfw3.h>

#include "Nebula/Application.h"

namespace Nebula {
	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		bool isDown = (state == GLFW_PRESS || state == GLFW_REPEAT);
		
		// Update current state
		m_KeyStates[keycode] = isDown;
		
		// Return true only if pressed this frame (down now, but wasn't down last frame)
		bool wasDown = m_PrevKeyStates.count(keycode) ? m_PrevKeyStates[keycode] : false;
		return isDown && !wasDown;
	}

	bool WindowsInput::IsKeyDownImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	
	void WindowsInput::UpdateImpl()
	{
		// Copy current frame states to previous frame
		m_PrevKeyStates = m_KeyStates;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;

	}

	float WindowsInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePosImpl();

		return (float)x;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePosImpl();

		return (float)y;
	}

	std::pair<float, float> WindowsInput::GetMousePosImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return std::pair<float, float>((float)xpos, (float)ypos);
	}
}
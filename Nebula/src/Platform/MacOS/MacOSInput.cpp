#include "nbpch.h"
#include "MacOSInput.h"

#include <GLFW/glfw3.h>

#include "Nebula/Application.h"

namespace Nebula {
	Input* Input::s_Instance = new MacOSInput();

	bool MacOSInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool MacOSInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}

	float MacOSInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePosImpl();

		return (float)x;
	}

	float MacOSInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePosImpl();

		return (float)y;
	}

	std::pair<float, float> MacOSInput::GetMousePosImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return std::pair<float, float>((float)xpos, (float)ypos);
	}
}

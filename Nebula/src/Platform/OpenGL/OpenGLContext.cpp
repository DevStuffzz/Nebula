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
	}

	void OpenGLContext::SwapBuffers()
	{

		glfwSwapBuffers(m_WindowHandle);
	}
}

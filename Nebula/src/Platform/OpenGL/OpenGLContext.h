#pragma once
#pragma warning(disable: 4251)


#include "Nebula/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Nebula {

	class OpenGLContext : public GraphicsContext {
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
	};

}
#pragma once
#pragma warning(disable: 4251)


namespace Nebula {
	class GraphicsContext {
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}
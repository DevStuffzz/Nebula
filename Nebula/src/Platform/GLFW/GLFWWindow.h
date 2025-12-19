#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Window.h"
#include "Nebula/Renderer/GraphicsContext.h "

#include <GLFW/glfw3.h>


namespace Nebula {

	class GLFWWindow : public Window {
	public:
		GLFWWindow(const WindowProps& props);
		virtual ~GLFWWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
	
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		bool ToggleFullscreen() override;
		void SetFullscreen(bool fullscreen);

		void SetCursorMode(bool disabled) override;

		inline void* GetNativeWindow() const override { return m_Window; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

		struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync, Fullscreen;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}
#pragma once
#pragma warning(disable: 4251)

#include "nbpch.h"


#include "Nebula/Core.h"
#include "Nebula/Events/Event.h"

namespace Nebula {
	
	struct WindowProps {
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		bool Fullscreen;

		WindowProps(const std::string& title = "Nebula Engine", unsigned int width = 1920, unsigned int height = 1080)
			: Title(title), Width(width), Height(height), Fullscreen(false) {}
	};

	// Interface representing a desktop system based Window
	class NEBULA_API Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		// Simple Toggle for Fullscreen. Returns window state for convinience (true=fullscreen on | false=fullscreen off)
		virtual bool ToggleFullscreen() = 0;
		// Sets the current Window to fullscreen
		virtual void SetFullscreen(bool fullscreen) = 0;

		virtual void SetCursorMode(bool disabled) = 0;
		
		// Returns the Native Window (ie GLFWwindow* for GLFW, HWIND for DX etc)
		virtual void* GetNativeWindow() const = 0;
		
		// Creates Window based on the current platform; GLFW only for now.
		static Window* Create(const WindowProps& props = WindowProps());
	};
}
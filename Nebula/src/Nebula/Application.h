#pragma once

#include "Core.h"

#include "Nebula/Events/ApplicationEvent.h"
#include "Window.h"

#include "LayerStack.h"


namespace Nebula {
	class NEBULA_API Application
	{
	public:
		Application();
		~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

	private:
		bool OnWindowClose(WindowCloseEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;

		LayerStack m_LayerStack;
	};

	// To be defined in Client
	Application* CreateApplication();


}


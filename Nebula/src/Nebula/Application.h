#pragma once

#include "Core.h"
#include "Nebula/Events/ApplicationEvent.h"
#include "Events/Event.h"
#include "Window.h"



namespace Nebula {
	class NEBULA_API Application
	{
	public:
		Application();
		~Application();

		void Run();

		void OnEvent(Event& e);

	private:
		bool OnWindowClose(WindowCloseEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// To be defined in Client
	Application* CreateApplication();


}


#pragma once

#include "Core.h"

#include "Nebula/Events/ApplicationEvent.h"
#include "Window.h"

#include "LayerStack.h"



namespace Nebula {

	class ImGuiLayer;

	class NEBULA_API Application
	{
	public:
		Application();
		~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;

		LayerStack m_LayerStack;
		static Application* s_Instance;
	};

	// To be defined in Client
	Application* CreateApplication();


}


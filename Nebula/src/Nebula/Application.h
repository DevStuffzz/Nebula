#pragma once
#pragma warning(disable: 4251)

#include "Core.h"

#include "Nebula/Events/ApplicationEvent.h"
#include "Window.h"
#include "Renderer/Camera.h"

#include "LayerStack.h"

namespace Nebula {

	class ImGuiLayer;

	class NEBULA_API Application
	{
	public:
		Application(bool enableDocking = true);
		~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }
	inline Camera& GetCamera() { return *m_Camera; }

	inline static Application& Get() { return *s_Instance; }

private:
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);

private:
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<Camera> m_Camera;
	ImGuiLayer* m_ImGuiLayer;
	bool m_Running = true;		LayerStack m_LayerStack;
		
		float m_LastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
	};

	// To be defined in Client
	Application* CreateApplication(int argc, char** argv);


}


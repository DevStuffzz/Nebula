#include "nbpch.h"

#include "Application.h"

#include "Nebula/Log.h"
#include "Nebula/ImGui/ImGuiLayer.h"
#include "Nebula/Renderer/Renderer.h"
#include "Nebula/Asset/AssetManager.h"
#include "Nebula/Asset/AssetManagerRegistry.h"
#include "Nebula/Scripting/LuaScriptEngine.h"

#include <GLFW/glfw3.h>

#include "Nebula/Input.h"
#include "Nebula/Keycodes.h"



#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)


namespace Nebula {

	Application* Application::s_Instance = nullptr;



	Application::Application()
	{
		NEB_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		// Initialize perspective camera with initial window aspect ratio
		uint32_t width = m_Window->GetWidth();
		uint32_t height = m_Window->GetHeight();
		float aspectRatio = (float)width / (float)height;
		m_Camera = std::unique_ptr<Camera>(new PerspectiveCamera(45.0f, aspectRatio, 0.1f, 100.0f));

		Renderer::Init();

		// Initialize Asset Management System
		AssetManager::Init();
		AssetManagerRegistry::RegisterImporters();

		// Initialize Lua Scripting System
		LuaScriptEngine::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		LuaScriptEngine::Shutdown();
		AssetManager::Shutdown();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
			(*--it)->OnEvent(e);
			if (e.Handled()) break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}
	
	void Application::Run() {
		while (m_Running) {
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			RenderCommand::Clear();

			for (Layer* layer : m_LayerStack) {
				layer->OnUpdate(timestep);
			}

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack) {
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();

			if(Input::IsKeyPressed(NB_KEY_ESCAPE))
				m_Running = false;
		}
	}


	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
		
		// Update camera aspect ratio
		if (m_Camera) {
			float aspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
			if (auto* perspCam = dynamic_cast<PerspectiveCamera*>(m_Camera.get())) {
				perspCam->SetProjection(45.0f, aspectRatio, 0.1f, 100.0f);
			}
		}
		
		return false;
	}
}
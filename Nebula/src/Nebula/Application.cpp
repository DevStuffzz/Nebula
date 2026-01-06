#include "nbpch.h"

#include "Application.h"

#include "Nebula/Log.h"
#include "Nebula/ImGui/ImGuiLayer.h"
#include "Nebula/Renderer/Renderer.h"
#include "Nebula/Asset/AssetManager.h"
#include "Nebula/Asset/AssetManagerRegistry.h"
#include "Nebula/Scripting/ScriptEngine.h"

#include <GLFW/glfw3.h>

#include "Nebula/Input.h"
#include "Nebula/Keycodes.h"



#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)


namespace Nebula {

	Application* Application::s_Instance = nullptr;


	Application::Application(bool enableDocking)
	{
		NEB_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		// Initialize camera
		uint32_t width = m_Window->GetWidth();
		uint32_t height = m_Window->GetHeight();
		float aspectRatio = (float)width / (float)height;
		m_Camera = std::unique_ptr<Camera>(new PerspectiveCamera(45.0f, aspectRatio, 0.1f, 100.0f));

		Renderer::Init();

		AssetManager::Init();
		AssetManagerRegistry::RegisterImporters();

	ScriptEngine::Init();

	m_ImGuiLayer = new ImGuiLayer(enableDocking);
	PushOverlay(m_ImGuiLayer);
}

Application::~Application()
{
	ScriptEngine::Shutdown();
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

			// Update time values
			m_UnscaledDeltaTime = timestep;
			m_UnscaledTime = time;
			m_DeltaTime = timestep * m_TimeScale;
			m_Time += m_DeltaTime;
			m_FrameCount++;

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

			if(Input::IsKeyPressed(NB_KEY_LEFT_ALT, NB_KEY_F4) )
				m_Running = false;

			if (Input::IsKeyPressed(NB_KEY_F11))
				m_Window->ToggleFullscreen();
				
			Input::Update();
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
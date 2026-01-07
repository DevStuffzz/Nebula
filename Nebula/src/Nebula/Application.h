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

		// Time management
		inline float GetDeltaTime() const { return m_DeltaTime; }
		inline float GetTime() const { return m_Time; }
		inline float GetUnscaledTime() const { return m_UnscaledTime; }
		inline float GetUnscaledDeltaTime() const { return m_UnscaledDeltaTime; }
		inline float GetTimeScale() const { return m_TimeScale; }
		inline void SetTimeScale(float scale) { m_TimeScale = scale; }
		inline int GetFrameCount() const { return m_FrameCount; }

		inline void SetRunning(bool running) { m_Running = running; }

		virtual void Exit(int code = 0) = 0;
	protected:
		LayerStack m_LayerStack;

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<Camera> m_Camera;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;

		float m_LastFrameTime = 0.0f;
		float m_DeltaTime = 0.0f;
		float m_Time = 0.0f;
		float m_UnscaledTime = 0.0f;
		float m_UnscaledDeltaTime = 0.0f;
		float m_TimeScale = 1.0f;
		int m_FrameCount = 0;

	private:
		static Application* s_Instance;
	};

	// To be defined in Client
	Application* CreateApplication(int argc, char** argv);


}


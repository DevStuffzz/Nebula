

#include <Nebula.h>

class RuntimeLayer : public Nebula::Layer {
public:
	RuntimeLayer() 
		: Layer("Runtime")
	{
		// Load scene manager's scene list
		Nebula::SceneManager::Get().LoadSceneList();
		
		// Load the first scene from the scene list
		auto scene = Nebula::SceneManager::Get().LoadScene(0);
		
		if (scene)
		{
			m_ActiveScene = scene;
			NB_INFO("Loaded scene: {0}", scene->GetName());
		}
		else
		{
			NB_ERROR("Failed to load scene from scene list. Creating default scene.");
			// Fallback to a default scene if loading fails

		}
	}

	void OnUpdate(Nebula::Timestep ts) override {
		// Calculate FPS
		m_FrameTime = ts.GetSeconds();
		m_FPS = 1.0f / m_FrameTime;

		// Update scene (runs all scripts, physics, etc.)
		if (m_ActiveScene)
		{
			m_ActiveScene->OnUpdate(ts);
		}

		// Find the primary camera in the scene
		Nebula::Camera* renderCamera = &Nebula::Application::Get().GetCamera();
		
		if (m_ActiveScene)
		{
			auto view = m_ActiveScene->GetRegistry().view<Nebula::CameraComponent, Nebula::TransformComponent>();
			for (auto entity : view)
			{
				auto& cameraComp = view.get<Nebula::CameraComponent>(entity);
				auto& transform = view.get<Nebula::TransformComponent>(entity);
				
				if (cameraComp.Primary)
				{
					// Update the application camera with the scene camera's settings
					auto& appCamera = static_cast<Nebula::PerspectiveCamera&>(Nebula::Application::Get().GetCamera());
					appCamera.SetPosition(transform.Position);
					appCamera.SetRotation(transform.Rotation);
					break;
				}
			}
		}

		// Render the scene to the window
		Nebula::Renderer::BeginScene(*renderCamera);

		if (m_ActiveScene)
		{
			m_ActiveScene->OnRender();
		}

		Nebula::Renderer::EndScene();
	}

	void OnImGuiRender() override {
#ifdef NB_DEBUG
		// Debug info panel
		Nebula::NebulaGui::Begin("Runtime Debug Info");
		
		// FPS counter
		Nebula::NebulaGui::Text("FPS: %.1f", m_FPS);
		Nebula::NebulaGui::Text("Frame Time: %.3f ms", m_FrameTime * 1000.0f);
		
		Nebula::NebulaGui::Separator();
		
		// Scene info
		if (m_ActiveScene)
		{
			Nebula::NebulaGui::Text("Scene: %s", m_ActiveScene->GetName().c_str());
			Nebula::NebulaGui::Text("Entity Count: %d", (int)m_ActiveScene->GetAllEntities().size());
			
			Nebula::NebulaGui::Separator();
			
			// List all entities
			if (Nebula::NebulaGui::CollapsingHeader("Entities"))
			{
				auto entities = m_ActiveScene->GetAllEntities();
				for (auto entity : entities)
				{
					if (entity.HasComponent<Nebula::TagComponent>())
					{
						auto& tag = entity.GetComponent<Nebula::TagComponent>().Tag;
						Nebula::NebulaGui::Text("- %s", tag.c_str());
					}
				}
			}
		}
		
		Nebula::NebulaGui::End();
#endif
	}

private:
	std::shared_ptr<Nebula::Scene> m_ActiveScene;
	float m_FPS = 0.0f;
	float m_FrameTime = 0.0f;
};

class Runtime : public Nebula::Application {
public:
	Runtime() {
		PushLayer(new RuntimeLayer());
	}

	~Runtime() {

	}
};



Nebula::Application* Nebula::CreateApplication() {
	return new Runtime();
}
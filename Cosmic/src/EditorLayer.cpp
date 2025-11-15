#include "EditorLayer.h"
#include "Nebula/Renderer/RenderCommand.h"
#include "Nebula/Log.h"
#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Application.h"
#include <Nebula/Scene/Components.h>

#include "Nebula/Renderer/Mesh.h"
#include <Nebula/Renderer/Shader.h>

#include "Nebula/Renderer/Material.h"

namespace Cosmic {

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_ActiveScene(nullptr)
	{
	}

	void EditorLayer::OnAttach()
	{
		NB_CORE_INFO("EditorLayer attached");

		Nebula::FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Nebula::Framebuffer::Create(fbSpec);

		// Position camera to see the scene
		auto& camera = Nebula::Application::Get().GetCamera();
		if (auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&camera)) {
			perspCam->SetPosition({ 0.0f, 0.0f, 5.0f });
		}

		// Create a default scene if none is set
		if (!m_ActiveScene)
		{
			m_ActiveScene = new Nebula::Scene("Editor Scene");
			
			// Create a sample entity
			auto entity = m_ActiveScene->CreateEntity("Sample Cube");
			auto& meshRenderer = entity.AddComponent<Nebula::MeshRendererComponent>();
			
			// Create mesh
			meshRenderer.Mesh = Nebula::Mesh::CreateCube();

			// Create shader and material
			Nebula::Shader* rawShader = Nebula::Shader::Create("assets/shaders/Basic.glsl");
			std::shared_ptr<Nebula::Shader> shader(rawShader);
			
			auto material = std::make_shared<Nebula::Material>(shader);
			material->SetFloat4("u_Color", glm::vec4(1.0f, 0.5f, 0.2f, 1.0f)); // Orange color
			material->SetInt("u_UseTexture", 0); // No texture
			
			meshRenderer.Material = material;
		}
	}

	void EditorLayer::OnDetach()
	{
		delete m_Framebuffer;
		
		// Only delete if we created it ourselves
		if (m_ActiveScene)
		{
			delete m_ActiveScene;
			m_ActiveScene = nullptr;
		}
	}

	void EditorLayer::OnUpdate(Nebula::Timestep ts)
	{
		// Resize framebuffer if needed
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f)
		{
			if (m_Framebuffer->GetSpecification().Width != (uint32_t)m_ViewportSize.x ||
				m_Framebuffer->GetSpecification().Height != (uint32_t)m_ViewportSize.y)
			{
				m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			}
		}

		// Render to framebuffer
		m_Framebuffer->Bind();
		Nebula::RenderCommand::SetViewport(0, 0, (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		Nebula::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Nebula::RenderCommand::Clear();

		// Update and render the active scene
		if (m_ActiveScene)
		{
			m_ActiveScene->OnUpdate(ts);
			m_ActiveScene->OnRender();
		}

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		// Simple viewport window for now
		Nebula::NebulaGui::Begin("Viewport");
		
		glm::vec2 viewportPanelSize = Nebula::NebulaGui::GetContentRegionAvail();
		m_ViewportSize = viewportPanelSize;

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		Nebula::NebulaGui::Image((void*)(intptr_t)textureID, glm::vec2{ m_ViewportSize.x, m_ViewportSize.y }, glm::vec2{ 0, 1 }, glm::vec2{ 1, 0 });

		Nebula::NebulaGui::End();
	}

	void EditorLayer::OnEvent(Nebula::Event& e)
	{
	}

}

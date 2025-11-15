#pragma once

#include "Nebula/Layer.h"
#include "Nebula/Timestep.h"
#include "Nebula/Events/Event.h"
#include "Nebula/Renderer/Framebuffer.h"
#include "Nebula/Scene/Scene.h"
#include <glm/glm.hpp>

namespace Cosmic {

	class EditorLayer : public Nebula::Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Nebula::Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Nebula::Event& e) override;

	private:
		Nebula::Framebuffer* m_Framebuffer;
		Nebula::Scene* m_ActiveScene;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

	public:
		// Allow setting a scene from outside
		void SetActiveScene(Nebula::Scene* scene) { m_ActiveScene = scene; }
		Nebula::Scene* GetActiveScene() { return m_ActiveScene; }
	};

}

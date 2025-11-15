#pragma once

#include <Nebula.h>

namespace Sandbox {

	class SceneLayer : public Nebula::Layer
	{
	public:
		SceneLayer();
		virtual ~SceneLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Nebula::Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Nebula::Event& e) override;

		// Get the scene for sharing with editor
		Nebula::Scene* GetScene() { return m_Scene; }

	private:
		void SetupExampleScene();

	private:
		Nebula::Scene* m_Scene;
		
		// Camera control
		glm::vec3 m_CameraPosition;
		glm::vec3 m_CameraRotation;
		float m_LastMouseX;
		float m_LastMouseY;
		bool m_FirstMouse;
	};

}

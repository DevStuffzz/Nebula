#pragma once

#include "Nebula/Layer.h"
#include "Nebula/Timestep.h"
#include "Nebula/Events/Event.h"
#include "Nebula/Renderer/Framebuffer.h"
#include "Nebula/Scene/Scene.h"
#include "Nebula/Scene/Entity.h"
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
		void NewScene();
		void SaveScene();
		void LoadScene();
		void LoadSceneFromPath(const std::string& filepath);
		void ToggleRuntime();

	private:
		Nebula::Framebuffer* m_Framebuffer;
		std::shared_ptr<Nebula::Scene> m_ActiveScene;
		std::string m_CurrentScenePath; // Track current scene file path
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		bool m_RuntimeMode = false;
		bool m_ShowScriptEditor = false;
	private:
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 5.0f };
		glm::vec3 m_CameraRotation = { 0.0f, 0.0f, 0.0f }; // pitch (x), yaw (y), roll (z)
		float m_LastMouseX = 0.0f;
		float m_LastMouseY = 0.0f;
		bool m_FirstMouse = true;

	// Gizmo state
	enum class GizmoOperation { None = -1, Translate = 0, Rotate = 1, Scale = 2 };
	GizmoOperation m_GizmoOperation = GizmoOperation::None;
	enum class GizmoAxis { None = -1, X = 0, Y = 1, Z = 2, XY = 3, XZ = 4, YZ = 5 };
	GizmoAxis m_ActiveAxis = GizmoAxis::None;
		bool m_IsUsingGizmo = false;
		glm::vec3 m_GizmoStartPos;
		glm::vec2 m_MouseStartPos;
		glm::vec3 m_DragStartIntersection;

	private:
		void RenderGizmo();
		GizmoAxis GetHoveredAxis(const glm::vec2& mousePos);

	public:
		// Allow setting a scene from outside
		void SetActiveScene(std::shared_ptr<Nebula::Scene> scene) { m_ActiveScene = scene; }
		std::shared_ptr<Nebula::Scene> GetActiveScene() { return m_ActiveScene; }
	};

}

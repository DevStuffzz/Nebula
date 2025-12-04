#pragma once

#include <glm/glm.hpp>
#include <functional>

namespace Nebula {
	class Entity;
	class PerspectiveCamera;
}

namespace Cosmic {

	class Gizmo
	{
	public:
		enum class Mode { None = 0, Translate, Rotate, Scale };
		enum class Axis { None = -1, X = 0, Y = 1, Z = 2, XY = 3, XZ = 4, YZ = 5, Screen = 6 };

		Gizmo();
		~Gizmo() = default;

		// Render the gizmo for the given entity
		void Render(Nebula::Entity& entity, Mode mode, const glm::vec2 viewportBounds[2]);

		// Handle mouse interaction - returns true if gizmo is being used
		bool HandleInput(Nebula::Entity& entity, Mode mode, const glm::vec2& mousePos, 
						bool mousePressed, const glm::vec2 viewportBounds[2]);

		// Check if gizmo is currently being used
		bool IsInUse() const { return m_IsUsingGizmo; }

	private:
		// Get which axis the mouse is hovering over
		Axis GetHoveredAxis(Nebula::Entity& entity, Mode mode, const glm::vec2& mousePos, 
						   const glm::vec2 viewportBounds[2]);

		// Rendering helpers
		void RenderTranslateGizmo(const glm::vec3& origin, Axis hoveredAxis, 
								 void* drawList, const glm::vec2 viewportBounds[2],
								 std::function<glm::vec2(const glm::vec3&)> project);
		void RenderRotateGizmo(const glm::vec3& origin, const glm::vec3& rotation, Axis hoveredAxis,
							  void* drawList, const glm::vec2 viewportBounds[2],
							  std::function<glm::vec2(const glm::vec3&)> project);
		void RenderScaleGizmo(const glm::vec3& origin, Axis hoveredAxis,
							 void* drawList, const glm::vec2 viewportBounds[2],
							 std::function<glm::vec2(const glm::vec3&)> project);

		// State
		bool m_IsUsingGizmo = false;
		Axis m_ActiveAxis = Axis::None;
		glm::vec3 m_GizmoStartPos;
		glm::vec3 m_GizmoStartRot;
		glm::vec3 m_GizmoStartScale;
		glm::vec2 m_MouseStartPos;
		glm::vec3 m_DragStartIntersection;
	};

}

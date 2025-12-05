#pragma once

#include "Nebula/Core.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <memory>

namespace Nebula {
	// Point Light Component
	struct NEBULA_API PointLightComponent
	{
		glm::vec3 Position = glm::vec3(0.0f); // World position
		glm::vec3 Color = glm::vec3(1.0f);    // RGB color
		float Intensity = 1.0f;               // Light intensity
		float Radius = 5.0f;                  // Light radius (falloff)

		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
		PointLightComponent(const glm::vec3& position, const glm::vec3& color, float intensity, float radius)
			: Position(position), Color(color), Intensity(intensity), Radius(radius) {}
	};

	// Directional Light Component
	struct NEBULA_API DirectionalLightComponent
	{
		glm::vec3 Color = glm::vec3(1.0f);                  // RGB color
		float Intensity = 1.0f;                             // Light intensity

		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
		DirectionalLightComponent(const glm::vec3& color, float intensity)
			: Color(color), Intensity(intensity) {}
	};

	class Mesh;
	class Material;

	// Transform Component
	struct NEBULA_API TransformComponent
	{
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f); // Euler angles in degrees
		glm::vec3 Scale = glm::vec3(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& position)
			: Position(position) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));
			return glm::translate(glm::mat4(1.0f), Position)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	// Tag Component - Every entity has a name
	struct NEBULA_API TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	// Mesh Renderer Component
	struct NEBULA_API MeshRendererComponent
	{
		std::shared_ptr<Mesh> Mesh;
		std::shared_ptr<Material> Material;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
		MeshRendererComponent(const std::shared_ptr<Nebula::Mesh>& mesh, const std::shared_ptr<Nebula::Material>& material)
			: Mesh(mesh), Material(material) {}
	};

	// Camera Component
	struct NEBULA_API CameraComponent
	{
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };

		ProjectionType Type = ProjectionType::Perspective;
		
		// Perspective
		float PerspectiveFOV = 45.0f;
		float PerspectiveNear = 0.1f;
		float PerspectiveFar = 1000.0f;

		// Orthographic
		float OrthographicSize = 10.0f;
		float OrthographicNear = -1.0f;
		float OrthographicFar = 1.0f;

		bool Primary = true; // Is this the main camera?

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	// Script Component
	struct NEBULA_API ScriptComponent
	{
		std::string ScriptPath; // Path to Lua script file

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
		ScriptComponent(const std::string& scriptPath)
			: ScriptPath(scriptPath) {}
	};

}

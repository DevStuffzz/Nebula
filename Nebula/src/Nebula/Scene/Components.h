#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Core.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <memory>

// Forward declare Bullet types (keep them outside Nebula namespace)
class btCollisionShape;
class btRigidBody;

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
		std::string MeshSource; // Path to the mesh file

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

	// Script Variable for editor exposure
	struct NEBULA_API ScriptVariable
	{
		enum class Type { Float, Int, Bool, String, Vec3 };
		
		std::string Name;
		Type VarType;
		
		// Union-like storage (only one is used based on VarType)
		float FloatValue = 0.0f;
		int IntValue = 0;
		bool BoolValue = false;
		std::string StringValue;
		glm::vec3 Vec3Value = glm::vec3(0.0f);
		
		ScriptVariable() = default;
		ScriptVariable(const std::string& name, float value)
			: Name(name), VarType(Type::Float), FloatValue(value) {}
		ScriptVariable(const std::string& name, int value)
			: Name(name), VarType(Type::Int), IntValue(value) {}
		ScriptVariable(const std::string& name, bool value)
			: Name(name), VarType(Type::Bool), BoolValue(value) {}
	};

	// Script Component - C# script support
	struct NEBULA_API ScriptComponent
	{
		std::string ClassName; // Fully qualified C# class name (e.g., "MyScripts.PlayerController")

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
		ScriptComponent(const std::string& className)
			: ClassName(className) {}
	};

	// Box Collider Component
	struct NEBULA_API BoxColliderComponent
	{
		glm::vec3 Size = glm::vec3(4.0f);       // Full dimensions of the box (matches default cube mesh)
		glm::vec3 Offset = glm::vec3(0.0f);     // Offset from entity position

		// Runtime data (opaque pointer to avoid exposing Bullet)
		btCollisionShape* RuntimeShape = nullptr;
		glm::vec3 LastScale = glm::vec3(1.0f);  // Track last scale to detect changes
		
		glm::vec3 LastSize = glm::vec3(0.0f);   // Track last size to detect changes

		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;
		BoxColliderComponent(const glm::vec3& size)
			: Size(size) {}
	};

	// Sphere Collider Component
	struct NEBULA_API SphereColliderComponent
	{
		float Radius = 2.0f;  // Matches default sphere mesh radius
		glm::vec3 Offset = glm::vec3(0.0f);     // Offset from entity position

		// Runtime data (opaque pointer to avoid exposing Bullet)
		btCollisionShape* RuntimeShape = nullptr;
		glm::vec3 LastScale = glm::vec3(1.0f);  // Track last scale to detect changes
		float LastRadius = 0.0f;                // Track last radius to detect changes

		SphereColliderComponent() = default;
		SphereColliderComponent(const SphereColliderComponent&) = default;
		SphereColliderComponent(float radius)
			: Radius(radius) {}
	};

	// RigidBody Component
	struct NEBULA_API RigidBodyComponent
	{
		enum class BodyType { Static = 0, Dynamic = 1, Kinematic = 2 };

		BodyType Type = BodyType::Dynamic;
		float Mass = 1.0f;
		float LinearDrag = 0.0f;
		float AngularDrag = 0.05f;
		bool UseGravity = true;
		bool IsKinematic = false;
		bool FreezeRotation = false;

		// Runtime velocity (synced from physics)
		glm::vec3 LinearVelocity = glm::vec3(0.0f);
		glm::vec3 AngularVelocity = glm::vec3(0.0f);

		// Runtime data (opaque pointer to avoid exposing Bullet)
		btRigidBody* RuntimeBody = nullptr;

		RigidBodyComponent() = default;
		RigidBodyComponent(const RigidBodyComponent&) = default;
	};

	// Line Renderer Component - For debug visualization and line rendering
	struct NEBULA_API LineRendererComponent
	{
		std::vector<glm::vec3> Points;
		glm::vec3 Color = glm::vec3(1.0f); // Default white
		float Width = 1.0f;
		bool Loop = false; // Connect last point to first

		LineRendererComponent() = default;
		LineRendererComponent(const LineRendererComponent&) = default;
		LineRendererComponent(const glm::vec3& color)
			: Color(color) {}
	};

	// Skybox Component - For environment skybox rendering
	class Skybox;
	struct NEBULA_API SkyboxComponent
	{
		std::string DirectoryPath;
		std::shared_ptr<Skybox> SkyboxInstance = nullptr;

		SkyboxComponent() = default;
		SkyboxComponent(const SkyboxComponent&) = default;
		SkyboxComponent(const std::string& directoryPath)
			: DirectoryPath(directoryPath) {}
	};

	// Forward declare audio types
	class AudioClip;

	// Audio Source Component - Plays audio in 3D space or 2D
	struct NEBULA_API AudioSourceComponent
	{
		std::string AudioClipPath;
		std::shared_ptr<AudioClip> Clip = nullptr;

		float Volume = 1.0f;
		float Pitch = 1.0f;
		bool Loop = false;
		bool PlayOnAwake = false;
		bool Spatial = true; // 3D spatial audio vs 2D audio

		// Spatial audio parameters
		float RolloffFactor = 1.0f;
		float ReferenceDistance = 1.0f;
		float MaxDistance = 100.0f;

		// Runtime state
		uint32_t RuntimeSourceID = 0;
		bool IsPlaying = false;

		AudioSourceComponent() = default;
		AudioSourceComponent(const AudioSourceComponent&) = default;
		AudioSourceComponent(const std::string& clipPath)
			: AudioClipPath(clipPath) {}
	};

	// Audio Listener Component - Receives audio (typically on camera)
	struct NEBULA_API AudioListenerComponent
	{
		bool Active = true;

		AudioListenerComponent() = default;
		AudioListenerComponent(const AudioListenerComponent&) = default;
	};

}

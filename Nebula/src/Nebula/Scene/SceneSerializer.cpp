#include "nbpch.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "Components.h"
#include "Nebula/Log.h"
#include "Nebula/Renderer/Material.h"
#include "Nebula/Renderer/Mesh.h"
#include "Nebula/Renderer/Shader.h"
#include "Nebula/Renderer/Camera.h"
#include <nlohmann/json.hpp>
#include <fstream>

#include "Platform/OpenGL/OpenGLTexture.h"

using json = nlohmann::json;

namespace Nebula {

	SceneSerializer::SceneSerializer(Scene* scene)
		: m_Scene(scene)
	{
	}

	// Helper function to serialize a vec3
	static json SerializeVec3(const glm::vec3& vec)
	{
		return json::array({ vec.x, vec.y, vec.z });
	}

	// Helper function to deserialize a vec3
	static glm::vec3 DeserializeVec3(const json& j)
	{
		return glm::vec3(j[0], j[1], j[2]);
	}

	// Helper function to serialize a vec4
	static json SerializeVec4(const glm::vec4& vec)
	{
		return json::array({ vec.x, vec.y, vec.z, vec.w });
	}

	// Helper function to deserialize a vec4
	static glm::vec4 DeserializeVec4(const json& j)
	{
		return glm::vec4(j[0], j[1], j[2], j[3]);
	}

	static void SerializeEntity(json& entityJson, Entity entity)
	{
		// Point Light Component
		if (entity.HasComponent<PointLightComponent>())
		{
			auto& light = entity.GetComponent<PointLightComponent>();
			json lightJson;
			lightJson["Position"] = SerializeVec3(light.Position);
			lightJson["Color"] = SerializeVec3(light.Color);
			lightJson["Intensity"] = light.Intensity;
			lightJson["Radius"] = light.Radius;
			entityJson["PointLightComponent"] = lightJson;
		}
		// Tag Component (always present)
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>();
			entityJson["TagComponent"]["Tag"] = tag.Tag;
		}

		// Transform Component (always present)
		if (entity.HasComponent<TransformComponent>())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			entityJson["TransformComponent"]["Position"] = SerializeVec3(transform.Position);
			entityJson["TransformComponent"]["Rotation"] = SerializeVec3(transform.Rotation);
			entityJson["TransformComponent"]["Scale"] = SerializeVec3(transform.Scale);
		}

		// Mesh Renderer Component
		if (entity.HasComponent<MeshRendererComponent>())
		{
			auto& meshRenderer = entity.GetComponent<MeshRendererComponent>();
			
			json meshRendererJson;
			meshRendererJson["HasMesh"] = (meshRenderer.Mesh != nullptr);
			meshRendererJson["HasMaterial"] = (meshRenderer.Material != nullptr);
			
			// Serialize material properties if material exists
			if (meshRenderer.Material)
			{
				auto& material = meshRenderer.Material;
				
				// Serialize material color
				glm::vec4 color = material->GetFloat4("u_Color");
				meshRendererJson["Material"]["Color"] = SerializeVec4(color);
				
				// Serialize material properties
				meshRendererJson["Material"]["Metallic"] = material->GetFloat("u_Metallic");
				meshRendererJson["Material"]["Roughness"] = material->GetFloat("u_Roughness");
				
				// Serialize texture usage
				meshRendererJson["Material"]["UseTexture"] = material->GetInt("u_UseTexture");
				
				// TODO: Serialize texture path/handle when asset system is fully integrated
				auto texture = material->GetTexture("u_Texture");
				if (texture)
				{
					meshRendererJson["Material"]["HasTexture"] = true;
                    // Save the texture path if available
                    // Only works for OpenGLTexture2D currently
					const auto* oglTexture = dynamic_cast<Nebula::OpenGLTexture2D*>(texture.get());
					if (oglTexture)
						meshRendererJson["Material"]["TexturePath"] = oglTexture->GetPath();
				}
				else
				{
					meshRendererJson["Material"]["HasTexture"] = false;
				}

			// Serialize mesh filename
			std::string meshFile = "";
			if (entity.HasComponent<TagComponent>()) {
				std::string tag = entity.GetComponent<TagComponent>().Tag;
				if (tag.find("Cube") != std::string::npos)
					meshFile = "Cube.obj";
				else if (tag.find("Sphere") != std::string::npos)
					meshFile = "Sphere.obj";
				else if (tag.find("Quad") != std::string::npos)
					meshFile = "Quad.obj";
			}
			meshRendererJson["MeshFile"] = meshFile;
			}
			
			entityJson["MeshRendererComponent"] = meshRendererJson;
		}

		// Camera Component
		if (entity.HasComponent<CameraComponent>())
		{
			auto& camera = entity.GetComponent<CameraComponent>();
			
			json cameraJson;
			cameraJson["Primary"] = camera.Primary;
			cameraJson["ProjectionType"] = (camera.Type == CameraComponent::ProjectionType::Perspective) ? "Perspective" : "Orthographic";
			
			// Perspective properties
			cameraJson["PerspectiveFOV"] = camera.PerspectiveFOV;
			cameraJson["PerspectiveNear"] = camera.PerspectiveNear;
			cameraJson["PerspectiveFar"] = camera.PerspectiveFar;
			
			// Orthographic properties
			cameraJson["OrthographicSize"] = camera.OrthographicSize;
			cameraJson["OrthographicNear"] = camera.OrthographicNear;
			cameraJson["OrthographicFar"] = camera.OrthographicFar;
			
			entityJson["CameraComponent"] = cameraJson;
		}

		// Script Component
		if (entity.HasComponent<ScriptComponent>())
		{
			auto& script = entity.GetComponent<ScriptComponent>();
			entityJson["ScriptComponent"]["ClassName"] = script.ClassName;
		}
	}

	static void DeserializeEntity(const json& entityJson, Scene* scene)
	{
		// Create entity
		std::string name = "Entity";
		if (entityJson.contains("TagComponent"))
		{
			name = entityJson["TagComponent"]["Tag"];
		}
		Entity entity = scene->CreateEntity(name);

		// Point Light Component
		if (entityJson.contains("PointLightComponent"))
		{
			const auto& lightJson = entityJson["PointLightComponent"];
			auto& light = entity.AddComponent<PointLightComponent>();
			light.Position = DeserializeVec3(lightJson["Position"]);
			light.Color = DeserializeVec3(lightJson["Color"]);
			light.Intensity = lightJson.value("Intensity", 1.0f);
			light.Radius = lightJson.value("Radius", 5.0f);
		}

		// Transform Component
		if (entityJson.contains("TransformComponent"))
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			transform.Position = DeserializeVec3(entityJson["TransformComponent"]["Position"]);
			transform.Rotation = DeserializeVec3(entityJson["TransformComponent"]["Rotation"]);
			transform.Scale = DeserializeVec3(entityJson["TransformComponent"]["Scale"]);
		}

		// Mesh Renderer Component
		if (entityJson.contains("MeshRendererComponent"))
		{
			auto& meshRenderer = entity.AddComponent<MeshRendererComponent>();
			
			const auto& meshRendererJson = entityJson["MeshRendererComponent"];
			
			bool hasMesh = meshRendererJson.value("HasMesh", false);
			if (hasMesh)
			{
				std::string meshFile = meshRendererJson.value("MeshFile", "Cube.obj");
				meshRenderer.Mesh = Mesh::LoadOBJ("assets/models/" + meshFile);
			}
			
			// Restore material if it existed
			bool hasMaterial = meshRendererJson.value("HasMaterial", false);
			if (hasMaterial && meshRendererJson.contains("Material"))
			{
				// Create a basic shader (this should come from asset system in the future)
				Shader* shader = Shader::Create("assets/shaders/Basic.glsl");
				auto material = std::make_shared<Material>(std::shared_ptr<Shader>(shader));
				
				const auto& matJson = meshRendererJson["Material"];
				
				// Restore material properties
				if (matJson.contains("Color"))
				{
					glm::vec4 color = DeserializeVec4(matJson["Color"]);
					material->SetFloat4("u_Color", color);
				}
				
				if (matJson.contains("Metallic"))
				{
					material->SetFloat("u_Metallic", matJson["Metallic"]);
				}
				
				if (matJson.contains("Roughness"))
				{
					material->SetFloat("u_Roughness", matJson["Roughness"]);
				}
				
				if (matJson.contains("UseTexture"))
				{
					material->SetInt("u_UseTexture", matJson["UseTexture"]);
				}
				
				// Restore texture if path is available
				if (matJson.contains("TexturePath"))
				{
					std::string texturePath = matJson["TexturePath"];
					auto texture = std::shared_ptr<Nebula::Texture2D>(Nebula::Texture2D::Create(texturePath));
					material->SetTexture("u_Texture", texture);
				}
				
				meshRenderer.Material = material;
			}
		}

		// Camera Component
		if (entityJson.contains("CameraComponent"))
		{
			const auto& cameraJson = entityJson["CameraComponent"];
			auto& camera = entity.AddComponent<CameraComponent>();
			
			camera.Primary = cameraJson.value("Primary", true);
			
			std::string projectionType = cameraJson.value("ProjectionType", "Perspective");
			camera.Type = (projectionType == "Perspective") ? 
				CameraComponent::ProjectionType::Perspective : 
				CameraComponent::ProjectionType::Orthographic;
			
			// Perspective properties
			camera.PerspectiveFOV = cameraJson.value("PerspectiveFOV", 45.0f);
			camera.PerspectiveNear = cameraJson.value("PerspectiveNear", 0.1f);
			camera.PerspectiveFar = cameraJson.value("PerspectiveFar", 1000.0f);
			
			// Orthographic properties
			camera.OrthographicSize = cameraJson.value("OrthographicSize", 10.0f);
			camera.OrthographicNear = cameraJson.value("OrthographicNear", -1.0f);
			camera.OrthographicFar = cameraJson.value("OrthographicFar", 1.0f);
		}

		// Script Component
		if (entityJson.contains("ScriptComponent"))
		{
			const auto& scriptJson = entityJson["ScriptComponent"];
			auto& script = entity.AddComponent<ScriptComponent>();
			script.ClassName = scriptJson["ClassName"];
		}
	}

	bool SceneSerializer::Serialize(const std::string& filepath)
	{
		json sceneJson;
		
		// Scene metadata
		sceneJson["Scene"]["Name"] = m_Scene->GetName();
		sceneJson["Scene"]["Version"] = "1.0";
		
		// Serialize all entities
		json entitiesJson = json::array();
		
		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity{ entityID, m_Scene };
			
			if (!entity)
				return;
			
			json entityJson;
			entityJson["ID"] = (uint32_t)entityID;
			
			SerializeEntity(entityJson, entity);
			entitiesJson.push_back(entityJson);
		});
		
		sceneJson["Entities"] = entitiesJson;
		
		// Write to file
		std::ofstream file(filepath);
		if (!file.is_open())
		{
			NB_CORE_ERROR("Failed to open file for writing: {0}", filepath);
			return false;
		}
		
		// Write with pretty formatting (4 spaces indent)
		file << sceneJson.dump(4);
		file.close();
		
		NB_CORE_INFO("Scene serialized to: {0}", filepath);
		return true;
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		std::ifstream file(filepath);
		if (!file.is_open())
		{
			NB_CORE_ERROR("Failed to open scene file: {0}", filepath);
			return false;
		}
		
		json sceneJson;
		try
		{
			file >> sceneJson;
		}
		catch (json::parse_error& e)
		{
			NB_CORE_ERROR("Failed to parse scene JSON: {0}", e.what());
			return false;
		}
		
		file.close();
		
		// Clear existing scene
		m_Scene->Clear();
		
		// Deserialize scene metadata
		if (sceneJson.contains("Scene"))
		{
			std::string sceneName = sceneJson["Scene"].value("Name", "Untitled");
			m_Scene->SetName(sceneName);
		}
		
		// Deserialize entities
		if (sceneJson.contains("Entities"))
		{
			for (const auto& entityJson : sceneJson["Entities"])
			{
				DeserializeEntity(entityJson, m_Scene);
			}
		}
		
		NB_CORE_INFO("Scene deserialized from: {0}", filepath);
		return true;
	}

	std::string SceneSerializer::SerializeToString()
	{
		json sceneJson;
		
		sceneJson["Scene"]["Name"] = m_Scene->GetName();
		sceneJson["Scene"]["Version"] = "1.0";
		
		json entitiesJson = json::array();
		
		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity{ entityID, m_Scene };
			
			if (!entity)
				return;
			
			json entityJson;
			entityJson["ID"] = (uint32_t)entityID;
			
			SerializeEntity(entityJson, entity);
			entitiesJson.push_back(entityJson);
		});
		
		sceneJson["Entities"] = entitiesJson;
		
		return sceneJson.dump(4);
	}

}

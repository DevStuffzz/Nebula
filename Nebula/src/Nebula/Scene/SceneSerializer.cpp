#include "nbpch.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "Components.h"
#include "Nebula/Log.h"
#include "Nebula/Renderer/Material.h"
#include "Nebula/Renderer/Mesh.h"
#include "Nebula/Renderer/Shader.h"
#include "Nebula/Renderer/Camera.h"
#include "Nebula/Physics/PhysicsWorld.h"
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

		// Directional Light Component
		if (entity.HasComponent<DirectionalLightComponent>())
		{
			auto& dirLight = entity.GetComponent<DirectionalLightComponent>();
			json dirLightJson;
			dirLightJson["Color"] = SerializeVec3(dirLight.Color);
			dirLightJson["Intensity"] = dirLight.Intensity;
			entityJson["DirectionalLightComponent"] = dirLightJson;
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
			
			// Serialize mesh ID and source path
			if (meshRenderer.Mesh)
			{
				meshRendererJson["MeshID"] = meshRenderer.Mesh->GetID();
				meshRendererJson["MeshSource"] = meshRenderer.Mesh->GetSourcePath();
			}
			
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
					{
						meshRendererJson["Material"]["TexturePath"] = oglTexture->GetPath();
						meshRendererJson["Material"]["TextureFilterNearest"] = oglTexture->GetUseNearest();
						meshRendererJson["Material"]["TextureRepeat"] = oglTexture->GetRepeat();
					}
					
					// Save texture tiling if set
					glm::vec2 tiling = material->GetFloat2("u_TextureTiling");
					meshRendererJson["Material"]["TextureTiling"] = {tiling.x, tiling.y};
				}
				else
				{
					meshRendererJson["Material"]["HasTexture"] = false;
				}
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
			json scriptJson;
			
			// Save all script paths
			scriptJson["ScriptPaths"] = script.ScriptPaths;
			
			// Save variables
			json varsJson = json::array();
			for (const auto& var : script.Variables)
			{
				json varJson;
				varJson["Name"] = var.Name;
				varJson["Type"] = (int)var.VarType;
				
				switch (var.VarType)
				{
				case ScriptVariable::Type::Float:
					varJson["Value"] = var.FloatValue;
					break;
				case ScriptVariable::Type::Int:
					varJson["Value"] = var.IntValue;
					break;
				case ScriptVariable::Type::Bool:
					varJson["Value"] = var.BoolValue;
					break;
				case ScriptVariable::Type::Vec3:
					varJson["Value"] = { var.Vec3Value.x, var.Vec3Value.y, var.Vec3Value.z };
					break;
				default:
					break;
				}
				varsJson.push_back(varJson);
			}
			scriptJson["Variables"] = varsJson;
			
			entityJson["ScriptComponent"] = scriptJson;
		}

		// Box Collider Component
		if (entity.HasComponent<BoxColliderComponent>())
		{
			auto& collider = entity.GetComponent<BoxColliderComponent>();
			entityJson["BoxColliderComponent"]["Size"] = { collider.Size.x, collider.Size.y, collider.Size.z };
			entityJson["BoxColliderComponent"]["Offset"] = { collider.Offset.x, collider.Offset.y, collider.Offset.z };
		}

		// Sphere Collider Component
		if (entity.HasComponent<SphereColliderComponent>())
		{
			auto& collider = entity.GetComponent<SphereColliderComponent>();
			entityJson["SphereColliderComponent"]["Radius"] = collider.Radius;
			entityJson["SphereColliderComponent"]["Offset"] = { collider.Offset.x, collider.Offset.y, collider.Offset.z };
		}

		// RigidBody Component
		if (entity.HasComponent<RigidBodyComponent>())
		{
			auto& rb = entity.GetComponent<RigidBodyComponent>();
			entityJson["RigidBodyComponent"]["Type"] = (int)rb.Type;
			entityJson["RigidBodyComponent"]["Mass"] = rb.Mass;
			entityJson["RigidBodyComponent"]["LinearDrag"] = rb.LinearDrag;
			entityJson["RigidBodyComponent"]["AngularDrag"] = rb.AngularDrag;
			entityJson["RigidBodyComponent"]["UseGravity"] = rb.UseGravity;
			entityJson["RigidBodyComponent"]["IsKinematic"] = rb.IsKinematic;
			entityJson["RigidBodyComponent"]["FreezeRotation"] = rb.FreezeRotation;
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

		// Directional Light Component
		if (entityJson.contains("DirectionalLightComponent"))
		{
			const auto& dirLightJson = entityJson["DirectionalLightComponent"];
			auto& dirLight = entity.AddComponent<DirectionalLightComponent>();
			dirLight.Color = DeserializeVec3(dirLightJson["Color"]);
			dirLight.Intensity = dirLightJson.value("Intensity", 1.0f);
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
				// Try to load by MeshID first (new system)
				if (meshRendererJson.contains("MeshID"))
				{
					MeshID meshID = meshRendererJson["MeshID"];
					meshRenderer.Mesh = Mesh::GetByID(meshID);
					
					// If not in registry, load from source path
					if (!meshRenderer.Mesh && meshRendererJson.contains("MeshSource"))
					{
						std::string meshSource = meshRendererJson["MeshSource"];
						meshRenderer.Mesh = Mesh::LoadOBJ(meshSource);
					}
				}
				// Fallback to old MeshFile system for backward compatibility
				else if (meshRendererJson.contains("MeshFile"))
				{
					std::string meshFile = meshRendererJson["MeshFile"];
					if (!meshFile.empty())
						meshRenderer.Mesh = Mesh::LoadOBJ("assets/models/" + meshFile);
				}
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
					bool useNearest = matJson.value("TextureFilterNearest", false);
					bool repeat = matJson.value("TextureRepeat", true);
					auto texture = std::shared_ptr<Nebula::Texture2D>(Nebula::Texture2D::Create(texturePath, useNearest, repeat));
					material->SetTexture("u_Texture", texture);
					
					// Restore texture tiling if set
					if (matJson.contains("TextureTiling"))
					{
						auto tiling = matJson["TextureTiling"];
						material->SetFloat2("u_TextureTiling", glm::vec2(tiling[0], tiling[1]));
					}
					else
					{
						// Default tiling to 1.0, 1.0
						material->SetFloat2("u_TextureTiling", glm::vec2(1.0f, 1.0f));
					}
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
			
			// Load script paths (support both old single path and new multiple paths)
			if (scriptJson.contains("ScriptPaths"))
			{
				script.ScriptPaths = scriptJson["ScriptPaths"].get<std::vector<std::string>>();
			}
			else if (scriptJson.contains("ScriptPath"))
			{
				// Backward compatibility with single script
				std::string path = scriptJson["ScriptPath"];
				if (!path.empty())
					script.ScriptPaths.push_back(path);
			}
			else if (scriptJson.contains("ClassName"))
			{
				// Legacy support
				std::string path = "assets/scripts/" + std::string(scriptJson["ClassName"]) + ".lua";
				script.ScriptPaths.push_back(path);
			}
			
			// Load variables
			if (scriptJson.contains("Variables"))
			{
				for (const auto& varJson : scriptJson["Variables"])
				{
					ScriptVariable var;
					var.Name = varJson["Name"];
					var.VarType = (ScriptVariable::Type)varJson["Type"].get<int>();
					
					switch (var.VarType)
					{
					case ScriptVariable::Type::Float:
						var.FloatValue = varJson["Value"];
						break;
					case ScriptVariable::Type::Int:
						var.IntValue = varJson["Value"];
						break;
					case ScriptVariable::Type::Bool:
						var.BoolValue = varJson["Value"];
						break;
					case ScriptVariable::Type::Vec3:
						var.Vec3Value = glm::vec3(varJson["Value"][0], varJson["Value"][1], varJson["Value"][2]);
						break;
					default:
						break;
					}
					script.Variables.push_back(var);
				}
			}
		}

		// Box Collider Component
		if (entityJson.contains("BoxColliderComponent"))
		{
			const auto& colliderJson = entityJson["BoxColliderComponent"];
			auto& collider = entity.AddComponent<BoxColliderComponent>();
			if (colliderJson.contains("Size"))
			{
				collider.Size = glm::vec3(
					colliderJson["Size"][0],
					colliderJson["Size"][1],
					colliderJson["Size"][2]
				);
			}
			if (colliderJson.contains("Offset"))
			{
				collider.Offset = glm::vec3(
					colliderJson["Offset"][0],
					colliderJson["Offset"][1],
					colliderJson["Offset"][2]
				);
			}

			// Initialize collider in physics world
			if (entity.GetScene()->GetPhysicsWorld())
			{
				entity.GetScene()->GetPhysicsWorld()->AddBoxCollider(entity);
			}
		}

		// Sphere Collider Component
		if (entityJson.contains("SphereColliderComponent"))
		{
			const auto& colliderJson = entityJson["SphereColliderComponent"];
			auto& collider = entity.AddComponent<SphereColliderComponent>();
			if (colliderJson.contains("Radius"))
				collider.Radius = colliderJson["Radius"];
			if (colliderJson.contains("Offset"))
			{
				collider.Offset = glm::vec3(
					colliderJson["Offset"][0],
					colliderJson["Offset"][1],
					colliderJson["Offset"][2]
				);
			}

			// Initialize collider in physics world
			if (entity.GetScene()->GetPhysicsWorld())
			{
				entity.GetScene()->GetPhysicsWorld()->AddSphereCollider(entity);
			}
		}

		// RigidBody Component
		if (entityJson.contains("RigidBodyComponent"))
		{
			const auto& rbJson = entityJson["RigidBodyComponent"];
			auto& rb = entity.AddComponent<RigidBodyComponent>();
			
			if (rbJson.contains("Type"))
				rb.Type = (RigidBodyComponent::BodyType)(int)rbJson["Type"];
			if (rbJson.contains("Mass"))
				rb.Mass = rbJson["Mass"];
			if (rbJson.contains("LinearDrag"))
				rb.LinearDrag = rbJson["LinearDrag"];
			if (rbJson.contains("AngularDrag"))
				rb.AngularDrag = rbJson["AngularDrag"];
			if (rbJson.contains("UseGravity"))
				rb.UseGravity = rbJson["UseGravity"];
			if (rbJson.contains("IsKinematic"))
				rb.IsKinematic = rbJson["IsKinematic"];
			if (rbJson.contains("FreezeRotation"))
				rb.FreezeRotation = rbJson["FreezeRotation"];

			// Initialize rigidbody in physics world
			if (entity.GetScene()->GetPhysicsWorld())
			{
				entity.GetScene()->GetPhysicsWorld()->AddRigidBody(entity);
			}
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
		
		auto view = m_Scene->m_Registry.view<entt::entity>();
		for (auto entityID : view)
		{
			Entity entity{ entityID, m_Scene };
			
			if (!entity)
				continue;
			
			json entityJson;
			entityJson["ID"] = (uint32_t)entityID;
			
			SerializeEntity(entityJson, entity);
			entitiesJson.push_back(entityJson);
		}
		
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
		
		auto view = m_Scene->m_Registry.view<entt::entity>();
		for (auto entityID : view)
		{
			Entity entity{ entityID, m_Scene };
			
			if (!entity)
				continue;
			
			json entityJson;
			entityJson["ID"] = (uint32_t)entityID;
			
			SerializeEntity(entityJson, entity);
			entitiesJson.push_back(entityJson);
		}
		
		sceneJson["Entities"] = entitiesJson;
		
		return sceneJson.dump(4);
	}

}

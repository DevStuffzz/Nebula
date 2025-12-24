#pragma once

#include "Nebula/Scene/Entity.h"
#include "Nebula/Scene/Components.h"
#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Renderer/Material.h"
#include "Nebula/Renderer/Texture.h"
#include "Nebula/Renderer/Skybox.h"
// #include "Nebula/Scripting/LuaScriptEngine.h"  // TODO: Re-enable when C# scripting is fixed
#include "Platform/OpenGL/OpenGLTexture.h"
#include <glm/glm.hpp>
#include <memory>
#include <filesystem>

namespace Cosmic {

	class PropertiesPanel
	{
	public:
		static void SetSelectedEntity(Nebula::Entity entity)
		{
			s_SelectedEntity = entity;
		}

		static void OnImGuiRender()
		{
			Nebula::NebulaGui::Begin("Properties");

			if (s_SelectedEntity)
			{
				// Tag Component
				if (s_SelectedEntity.HasComponent<Nebula::TagComponent>())
				{
					DrawTagComponent(s_SelectedEntity.GetComponent<Nebula::TagComponent>());
				}

				// Transform Component
				if (s_SelectedEntity.HasComponent<Nebula::TransformComponent>())
				{
					DrawTransformComponent(s_SelectedEntity.GetComponent<Nebula::TransformComponent>());
				}

				// Mesh Renderer Component
				if (s_SelectedEntity.HasComponent<Nebula::MeshRendererComponent>())
					DrawMeshRendererComponent(s_SelectedEntity.GetComponent<Nebula::MeshRendererComponent>());

				// Point Light Component
				if (s_SelectedEntity.HasComponent<Nebula::PointLightComponent>())
					DrawPointLightComponent(s_SelectedEntity.GetComponent<Nebula::PointLightComponent>());

				// Directional Light Component
				if (s_SelectedEntity.HasComponent<Nebula::DirectionalLightComponent>())
					DrawDirectionalLightComponent(s_SelectedEntity.GetComponent<Nebula::DirectionalLightComponent>());

				// Camera Component
				if (s_SelectedEntity.HasComponent<Nebula::CameraComponent>())
					DrawCameraComponent(s_SelectedEntity.GetComponent<Nebula::CameraComponent>());

				// Script Component
				if (s_SelectedEntity.HasComponent<Nebula::ScriptComponent>())
					DrawScriptComponent(s_SelectedEntity.GetComponent<Nebula::ScriptComponent>());

				// RigidBody Component
				if (s_SelectedEntity.HasComponent<Nebula::RigidBodyComponent>())
					DrawRigidBodyComponent(s_SelectedEntity.GetComponent<Nebula::RigidBodyComponent>());

				// Box Collider Component
				if (s_SelectedEntity.HasComponent<Nebula::BoxColliderComponent>())
					DrawBoxColliderComponent(s_SelectedEntity.GetComponent<Nebula::BoxColliderComponent>());

				// Sphere Collider Component
				if (s_SelectedEntity.HasComponent<Nebula::SphereColliderComponent>())
					DrawSphereColliderComponent(s_SelectedEntity.GetComponent<Nebula::SphereColliderComponent>());

				// Skybox Component
				if (s_SelectedEntity.HasComponent<Nebula::SkyboxComponent>())
					DrawSkyboxComponent(s_SelectedEntity.GetComponent<Nebula::SkyboxComponent>());

				// Audio Source Component
				if (s_SelectedEntity.HasComponent<Nebula::AudioSourceComponent>())
					DrawAudioSourceComponent(s_SelectedEntity.GetComponent<Nebula::AudioSourceComponent>());

				// Audio Listener Component
				if (s_SelectedEntity.HasComponent<Nebula::AudioListenerComponent>())
					DrawAudioListenerComponent(s_SelectedEntity.GetComponent<Nebula::AudioListenerComponent>());

				Nebula::NebulaGui::Separator();

				// Add Component button
				if (Nebula::NebulaGui::Button("Add Component"))
				{
					Nebula::NebulaGui::OpenPopup("AddComponent");
				}

				if (Nebula::NebulaGui::BeginPopup("AddComponent"))
				{
					if (Nebula::NebulaGui::MenuItem("Mesh Renderer"))
					{
						if (!s_SelectedEntity.HasComponent<Nebula::MeshRendererComponent>())
							s_SelectedEntity.AddComponent<Nebula::MeshRendererComponent>();
						Nebula::NebulaGui::CloseCurrentPopup();
					}
					if (Nebula::NebulaGui::MenuItem("Point Light"))
					{
						if (!s_SelectedEntity.HasComponent<Nebula::PointLightComponent>())
							s_SelectedEntity.AddComponent<Nebula::PointLightComponent>();
						Nebula::NebulaGui::CloseCurrentPopup();
					}
					if (Nebula::NebulaGui::MenuItem("Directional Light"))
					{
						if (!s_SelectedEntity.HasComponent<Nebula::DirectionalLightComponent>())
							s_SelectedEntity.AddComponent<Nebula::DirectionalLightComponent>();
						Nebula::NebulaGui::CloseCurrentPopup();
					}
					if (Nebula::NebulaGui::MenuItem("Camera"))
					{
						if (!s_SelectedEntity.HasComponent<Nebula::CameraComponent>())
							s_SelectedEntity.AddComponent<Nebula::CameraComponent>();
						Nebula::NebulaGui::CloseCurrentPopup();
					}
					if (Nebula::NebulaGui::MenuItem("Script"))
					{
						if (!s_SelectedEntity.HasComponent<Nebula::ScriptComponent>())
							s_SelectedEntity.AddComponent<Nebula::ScriptComponent>();
						Nebula::NebulaGui::CloseCurrentPopup();
					}
					if (Nebula::NebulaGui::MenuItem("Rigidbody"))
					{
						if (!s_SelectedEntity.HasComponent<Nebula::RigidBodyComponent>())
							s_SelectedEntity.AddComponent<Nebula::RigidBodyComponent>();
						Nebula::NebulaGui::CloseCurrentPopup();
					}
					if (Nebula::NebulaGui::MenuItem("Box Collider"))
					{
						if (!s_SelectedEntity.HasComponent<Nebula::BoxColliderComponent>())
							s_SelectedEntity.AddComponent<Nebula::BoxColliderComponent>();
						Nebula::NebulaGui::CloseCurrentPopup();
					}
					if (Nebula::NebulaGui::MenuItem("Sphere Collider")) {
						if (!s_SelectedEntity.HasComponent<Nebula::SphereColliderComponent>())
							s_SelectedEntity.AddComponent<Nebula::SphereColliderComponent>();
						Nebula::NebulaGui::CloseCurrentPopup();
					}
					if (Nebula::NebulaGui::MenuItem("Skybox"))
					{
						if (!s_SelectedEntity.HasComponent<Nebula::SkyboxComponent>())
							s_SelectedEntity.AddComponent<Nebula::SkyboxComponent>();
						Nebula::NebulaGui::CloseCurrentPopup();
					}
					if (Nebula::NebulaGui::MenuItem("Audio Source"))
					{
						if (!s_SelectedEntity.HasComponent<Nebula::AudioSourceComponent>())
							s_SelectedEntity.AddComponent<Nebula::AudioSourceComponent>();
						Nebula::NebulaGui::CloseCurrentPopup();
					}
					if (Nebula::NebulaGui::MenuItem("Audio Listener"))
					{
						if (!s_SelectedEntity.HasComponent<Nebula::AudioListenerComponent>())
							s_SelectedEntity.AddComponent<Nebula::AudioListenerComponent>();
						Nebula::NebulaGui::CloseCurrentPopup();
					}
					Nebula::NebulaGui::EndPopup();
				}
			}
			else
			{
				Nebula::NebulaGui::Text("No entity selected");
			}

			Nebula::NebulaGui::End();
		}


	private:

		static void DrawTagComponent(Nebula::TagComponent& tag)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Tag", true))
			{
				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				strncpy_s(buffer, tag.Tag.c_str(), sizeof(buffer));
				
				if (Nebula::NebulaGui::InputText("Name", buffer, sizeof(buffer)))
				{
					tag.Tag = std::string(buffer);
				}
			}
		}

		static void DrawTransformComponent(Nebula::TransformComponent& transform)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Transform", true))
			{
				Nebula::NebulaGui::DragFloat3("Position", &transform.Position.x, 0.1f);
				Nebula::NebulaGui::DragFloat3("Rotation", &transform.Rotation.x, 0.5f);
				Nebula::NebulaGui::DragFloat3("Scale", &transform.Scale.x, 0.1f, 0.001f);
			}
		}

		static void DrawMeshRendererComponent(Nebula::MeshRendererComponent& meshRenderer)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Mesh Renderer", true))
			{
				Nebula::NebulaGui::SameLine(250.0f);
				if (Nebula::NebulaGui::Button("Remove##MeshRenderer"))
				{
					if (s_SelectedEntity.HasComponent<Nebula::MeshRendererComponent>())
						s_SelectedEntity.RemoveComponent<Nebula::MeshRendererComponent>();
					return;
				}
				
				Nebula::NebulaGui::Text("Mesh: %s", meshRenderer.Mesh ? "Loaded" : "None");
				Nebula::NebulaGui::SameLine();
				Nebula::NebulaGui::Button("[Drop Mesh]", glm::vec2(100, 30));
				if (Nebula::NebulaGui::BeginDragDropTarget())
				{
					const char* payload = (const char*)Nebula::NebulaGui::AcceptDragDropPayload("MESH_ASSET");
					if (payload)
					{
						std::string meshPath = payload;
						meshRenderer.Mesh = Nebula::Mesh::LoadOBJ(meshPath);
					}
					Nebula::NebulaGui::EndDragDropTarget();
				}
				Nebula::NebulaGui::Text("Material: %s", meshRenderer.Material ? "Loaded" : "None");

				if (meshRenderer.Material)
				{
					glm::vec4 color = meshRenderer.Material->GetFloat4("u_Color");
					if (Nebula::NebulaGui::ColorEdit4("Color", &color.x))
					{
						meshRenderer.Material->SetFloat4("u_Color", color);
					}

					Nebula::NebulaGui::Text("Texture:");
					Nebula::NebulaGui::SameLine();
					auto texture = meshRenderer.Material->GetTexture("u_Texture");
					if (texture)
					{
						Nebula::NebulaGui::Button("[Texture]", glm::vec2(100, 100));
					}
					else
					{
						Nebula::NebulaGui::Button("[Drop Here]", glm::vec2(100, 100));
					}
					if (Nebula::NebulaGui::BeginDragDropTarget())
					{
						const char* payload = (const char*)Nebula::NebulaGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
						if (payload)
						{
							std::string path = payload;
							auto hasExtension = [](const std::string& str, const std::string& ext) {
								return str.size() >= ext.size() && str.compare(str.size() - ext.size(), ext.size(), ext) == 0;
							};
							if (hasExtension(path, ".png") || hasExtension(path, ".jpg") || hasExtension(path, ".jpeg"))
							{
								std::shared_ptr<Nebula::Texture2D> newTexture;
								newTexture.reset(Nebula::Texture2D::Create(path));
								meshRenderer.Material->SetTexture("u_Texture", newTexture);
								meshRenderer.Material->SetInt("u_UseTexture", 1);
							}
						}
						Nebula::NebulaGui::EndDragDropTarget();
					}
					int useTexture = meshRenderer.Material->GetInt("u_UseTexture");
					bool useTextureBool = useTexture == 1;
					if (Nebula::NebulaGui::Checkbox("Use Texture", &useTextureBool))
					{
						meshRenderer.Material->SetInt("u_UseTexture", useTextureBool ? 1 : 0);
					}
					
					// Texture Tiling
					if (texture)
					{
						glm::vec2 tiling = meshRenderer.Material->GetFloat2("u_TextureTiling");
						if (Nebula::NebulaGui::DragFloat2("Texture Tiling", &tiling.x, 0.1f, 0.01f, 100.0f))
						{
							meshRenderer.Material->SetFloat2("u_TextureTiling", tiling);
						}
						
						// Texture Filtering and Wrapping
						const auto* oglTexture = dynamic_cast<Nebula::OpenGLTexture2D*>(texture.get());
						if (oglTexture)
						{
							bool useNearest = oglTexture->GetUseNearest();
							bool repeat = oglTexture->GetRepeat();
							
							bool filterChanged = false;
							bool wrapChanged = false;
							
							if (Nebula::NebulaGui::Checkbox("Nearest Filter", &useNearest))
								filterChanged = true;
							
							if (Nebula::NebulaGui::Checkbox("Repeat Wrap", &repeat))
								wrapChanged = true;
							
							// Reload texture if settings changed
							if (filterChanged || wrapChanged)
							{
								std::string texturePath = oglTexture->GetPath();
								std::shared_ptr<Nebula::Texture2D> newTexture;
								newTexture.reset(Nebula::Texture2D::Create(texturePath, useNearest, repeat));
								meshRenderer.Material->SetTexture("u_Texture", newTexture);
							}
						}
					}
				}
			}
		}

		static void DrawPointLightComponent(Nebula::PointLightComponent& light)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Point Light", true))
			{
				Nebula::NebulaGui::SameLine(250.0f);
				if (Nebula::NebulaGui::Button("Remove##PointLight"))
				{
					if (s_SelectedEntity.HasComponent<Nebula::PointLightComponent>())
						s_SelectedEntity.RemoveComponent<Nebula::PointLightComponent>();
					return;
				}
				
				Nebula::NebulaGui::ColorEdit3("Color", &light.Color.x);
				Nebula::NebulaGui::DragFloat("Intensity", &light.Intensity, 0.1f, 0.0f, 100.0f);
				Nebula::NebulaGui::DragFloat("Radius", &light.Radius, 0.1f, 0.0f, 100.0f);
			}
		}

		static void DrawDirectionalLightComponent(Nebula::DirectionalLightComponent& light)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Directional Light", true))
			{
				Nebula::NebulaGui::SameLine(250.0f);
				if (Nebula::NebulaGui::Button("Remove##DirectionalLight"))
				{
					if (s_SelectedEntity.HasComponent<Nebula::DirectionalLightComponent>())
						s_SelectedEntity.RemoveComponent<Nebula::DirectionalLightComponent>();
					return;
				}
				
				Nebula::NebulaGui::Text("Direction is controlled by entity rotation");
				Nebula::NebulaGui::ColorEdit3("Color", &light.Color.x);
				Nebula::NebulaGui::DragFloat("Intensity", &light.Intensity, 0.1f, 0.0f, 100.0f);
			}
		}

		static void DrawCameraComponent(Nebula::CameraComponent& camera)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Camera", true))
			{
				Nebula::NebulaGui::SameLine(250.0f);
				if (Nebula::NebulaGui::Button("Remove##Camera"))
				{
					if (s_SelectedEntity.HasComponent<Nebula::CameraComponent>())
						s_SelectedEntity.RemoveComponent<Nebula::CameraComponent>();
					return;
				}
				
				Nebula::NebulaGui::Checkbox("Primary", &camera.Primary);
				Nebula::NebulaGui::DragFloat("FOV", &camera.PerspectiveFOV, 0.1f, 1.0f, 179.0f);
				Nebula::NebulaGui::DragFloat("Near", &camera.PerspectiveNear, 0.01f, 0.001f, 100.0f);
				Nebula::NebulaGui::DragFloat("Far", &camera.PerspectiveFar, 0.1f, 0.1f, 10000.0f);
			}
		}

		static void DrawScriptComponent(Nebula::ScriptComponent& script)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Script", true))
			{
				Nebula::NebulaGui::SameLine(250.0f);
				if (Nebula::NebulaGui::Button("Remove##Script"))
				{
					if (s_SelectedEntity.HasComponent<Nebula::ScriptComponent>())
						s_SelectedEntity.RemoveComponent<Nebula::ScriptComponent>();
					return;
				}
				
				// Get all available script classes
				auto classNames = Nebula::ScriptEngine::GetEntityClassNames();
				
				// Find current selection index
				int currentIndex = -1;
				for (int i = 0; i < classNames.size(); i++)
				{
					if (classNames[i] == script.ClassName)
					{
						currentIndex = i;
						break;
					}
				}

				// Display as dropdown
				std::string previewText = script.ClassName.empty() ? "Select Script..." : script.ClassName;
				if (Nebula::NebulaGui::BeginCombo("Class Name", previewText.c_str()))
				{
					for (int i = 0; i < classNames.size(); i++)
					{
						bool isSelected = (i == currentIndex);
						if (Nebula::NebulaGui::Selectable(classNames[i].c_str(), isSelected))
						{
							script.ClassName = classNames[i];
						}
					}
					Nebula::NebulaGui::EndCombo();
				}
			}
		}

		static void DrawRigidBodyComponent(Nebula::RigidBodyComponent& rb)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Rigidbody", true))
			{
				Nebula::NebulaGui::SameLine(250.0f);
				if (Nebula::NebulaGui::Button("Remove##RigidBody"))
				{
					if (s_SelectedEntity.HasComponent<Nebula::RigidBodyComponent>())
						s_SelectedEntity.RemoveComponent<Nebula::RigidBodyComponent>();
					return;
				}
				
				// Body Type
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
				const char* currentBodyTypeString = bodyTypeStrings[(int)rb.Type];
				if (Nebula::NebulaGui::BeginCombo("Body Type", currentBodyTypeString))
				{
					for (int i = 0; i < 3; i++)
					{
						bool isSelected = (currentBodyTypeString == bodyTypeStrings[i]);
						if (Nebula::NebulaGui::Selectable(bodyTypeStrings[i], isSelected))
						{
							rb.Type = (Nebula::RigidBodyComponent::BodyType)i;
						}

						if (isSelected)
							Nebula::NebulaGui::SetItemDefaultFocus();
					}
					Nebula::NebulaGui::EndCombo();
				}

				// Mass (only for Dynamic)
				if (rb.Type == Nebula::RigidBodyComponent::BodyType::Dynamic)
				{
					Nebula::NebulaGui::DragFloat("Mass", &rb.Mass, 0.1f, 0.01f, 1000.0f);
				}

				// Drag
				Nebula::NebulaGui::DragFloat("Linear Drag", &rb.LinearDrag, 0.01f, 0.0f, 10.0f);
				Nebula::NebulaGui::DragFloat("Angular Drag", &rb.AngularDrag, 0.01f, 0.0f, 10.0f);

				// Flags
				Nebula::NebulaGui::Checkbox("Use Gravity", &rb.UseGravity);
				Nebula::NebulaGui::Checkbox("Freeze Rotation", &rb.FreezeRotation);

				// Display velocities (read-only)
				Nebula::NebulaGui::Separator();
				Nebula::NebulaGui::Text("Linear Velocity: (%.2f, %.2f, %.2f)", 
					rb.LinearVelocity.x, rb.LinearVelocity.y, rb.LinearVelocity.z);
				Nebula::NebulaGui::Text("Angular Velocity: (%.2f, %.2f, %.2f)", 
					rb.AngularVelocity.x, rb.AngularVelocity.y, rb.AngularVelocity.z);
			}
		}

		static void DrawBoxColliderComponent(Nebula::BoxColliderComponent& collider)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Box Collider", true))
			{
				Nebula::NebulaGui::SameLine(250.0f);
				if (Nebula::NebulaGui::Button("Remove##BoxCollider"))
				{
					if (s_SelectedEntity.HasComponent<Nebula::BoxColliderComponent>())
						s_SelectedEntity.RemoveComponent<Nebula::BoxColliderComponent>();
					return;
				}
				
				Nebula::NebulaGui::DragFloat3("Size", &collider.Size.x, 0.1f, 0.01f, 100.0f);
				Nebula::NebulaGui::DragFloat3("Offset", &collider.Offset.x, 0.1f);
			}
		}

		static void DrawSphereColliderComponent(Nebula::SphereColliderComponent& collider)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Sphere Collider", true))
			{
				Nebula::NebulaGui::SameLine(250.0f);
				if (Nebula::NebulaGui::Button("Remove##SphereCollider"))
				{
					if (s_SelectedEntity.HasComponent<Nebula::SphereColliderComponent>())
						s_SelectedEntity.RemoveComponent<Nebula::SphereColliderComponent>();
					return;
				}
				
				Nebula::NebulaGui::DragFloat("Radius", &collider.Radius, 0.1f, 0.01f, 100.0f);
				Nebula::NebulaGui::DragFloat3("Offset", &collider.Offset.x, 0.1f);
			}
		}

		static void DrawSkyboxComponent(Nebula::SkyboxComponent& skybox)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Skybox", true))
			{
				Nebula::NebulaGui::SameLine(250.0f);
				if (Nebula::NebulaGui::Button("Remove##Skybox"))
				{
					if (s_SelectedEntity.HasComponent<Nebula::SkyboxComponent>())
						s_SelectedEntity.RemoveComponent<Nebula::SkyboxComponent>();
					return;
				}

				// Display current directory path
				if (skybox.DirectoryPath.empty())
				{
					Nebula::NebulaGui::Text("Directory: None");
				}
				else
				{
					Nebula::NebulaGui::Text("Directory: %s", skybox.DirectoryPath.c_str());
				}

				// Drag and drop target for directory
				Nebula::NebulaGui::Button("[Drop Skybox Directory]", glm::vec2(200, 50));
				if (Nebula::NebulaGui::BeginDragDropTarget())
				{
					const char* payload = (const char*)Nebula::NebulaGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
					if (payload)
					{
						std::string path = payload;
						
						// Check if it's a directory
						if (std::filesystem::is_directory(path))
						{
							skybox.DirectoryPath = path;
							
							// Try to create the skybox
							try
							{
								skybox.SkyboxInstance = Nebula::Skybox::Create(path);
								if (!skybox.SkyboxInstance)
								{
									NB_CORE_ERROR("Failed to create skybox from directory: {0}", path);
								}
							}
							catch (const std::exception& e)
							{
								NB_CORE_ERROR("Exception creating skybox: {0}", e.what());
							}
						}
						else
						{
							NB_CORE_WARN("Skybox requires a directory, not a file. Please drag a folder containing skybox images.");
						}
					}
					Nebula::NebulaGui::EndDragDropTarget();
				}

				// Status indicator
				if (skybox.SkyboxInstance)
				{
					Nebula::NebulaGui::TextColored(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: Loaded");
				}
				else if (!skybox.DirectoryPath.empty())
				{
					Nebula::NebulaGui::TextColored(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), "Status: Failed to load");
				}
				else
				{
					Nebula::NebulaGui::TextColored(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), "Status: No directory selected");
				}

				// Reload button
				if (!skybox.DirectoryPath.empty() && Nebula::NebulaGui::Button("Reload Skybox"))
				{
					try
					{
						skybox.SkyboxInstance = Nebula::Skybox::Create(skybox.DirectoryPath);
					}
					catch (const std::exception& e)
					{
						NB_CORE_ERROR("Exception reloading skybox: {0}", e.what());
					}
				}
			}
		}

		static void DrawAudioSourceComponent(Nebula::AudioSourceComponent& audioSource)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Audio Source", true))
			{
				Nebula::NebulaGui::SameLine(250.0f);
				if (Nebula::NebulaGui::Button("Remove##AudioSource"))
				{
					if (s_SelectedEntity.HasComponent<Nebula::AudioSourceComponent>())
						s_SelectedEntity.RemoveComponent<Nebula::AudioSourceComponent>();
					return;
				}

				// Audio Clip Path
				Nebula::NebulaGui::Text("Audio Clip: %s", audioSource.AudioClipPath.empty() ? "None" : audioSource.AudioClipPath.c_str());
				Nebula::NebulaGui::Button("[Drop Audio File]", glm::vec2(150, 40));
				if (Nebula::NebulaGui::BeginDragDropTarget())
				{
					const char* payload = (const char*)Nebula::NebulaGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
					if (payload)
					{
						std::string path = payload;
						auto hasExtension = [](const std::string& str, const std::string& ext) {
							return str.size() >= ext.size() && str.compare(str.size() - ext.size(), ext.size(), ext) == 0;
						};
						if (hasExtension(path, ".wav") || hasExtension(path, ".ogg") || hasExtension(path, ".mp3"))
						{
							audioSource.AudioClipPath = path;
						}
						else
						{
							NB_CORE_WARN("Please drop a valid audio file (.wav, .ogg, .mp3)");
						}
					}
					Nebula::NebulaGui::EndDragDropTarget();
				}

				// Clear button
				if (!audioSource.AudioClipPath.empty())
				{
					Nebula::NebulaGui::SameLine();
					if (Nebula::NebulaGui::Button("Clear"))
					{
						audioSource.AudioClipPath = "";
					}
				}

				// Volume
				Nebula::NebulaGui::SliderFloat("Volume", &audioSource.Volume, 0.0f, 1.0f);

				// Pitch
				Nebula::NebulaGui::SliderFloat("Pitch", &audioSource.Pitch, 0.1f, 3.0f);

				// Loop
				Nebula::NebulaGui::Checkbox("Loop", &audioSource.Loop);

				// Play On Awake
				Nebula::NebulaGui::Checkbox("Play On Awake", &audioSource.PlayOnAwake);

				// Spatial
				Nebula::NebulaGui::Checkbox("Spatial (3D)", &audioSource.Spatial);

				if (audioSource.Spatial)
				{
					Nebula::NebulaGui::Text("  Spatial Audio Settings:");
					Nebula::NebulaGui::SliderFloat("  Rolloff Factor", &audioSource.RolloffFactor, 0.0f, 10.0f);
					Nebula::NebulaGui::SliderFloat("  Reference Distance", &audioSource.ReferenceDistance, 0.1f, 100.0f);
					Nebula::NebulaGui::SliderFloat("  Max Distance", &audioSource.MaxDistance, 1.0f, 1000.0f);
				}
			}
		}

		static void DrawAudioListenerComponent(Nebula::AudioListenerComponent& audioListener)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Audio Listener", true))
			{
				Nebula::NebulaGui::SameLine(250.0f);
				if (Nebula::NebulaGui::Button("Remove##AudioListener"))
				{
					if (s_SelectedEntity.HasComponent<Nebula::AudioListenerComponent>())
						s_SelectedEntity.RemoveComponent<Nebula::AudioListenerComponent>();
					return;
				}

				Nebula::NebulaGui::Checkbox("Active", &audioListener.Active);
				Nebula::NebulaGui::Text("The audio listener position is typically\nattached to the main camera.");
			}
		}

	private:
		static Nebula::Entity s_SelectedEntity;
	};

	inline Nebula::Entity PropertiesPanel::s_SelectedEntity = {};
}

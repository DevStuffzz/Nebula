#pragma once

#include "Nebula/Scene/Entity.h"
#include "Nebula/Scene/Components.h"
#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Renderer/Material.h"
#include "Nebula/Renderer/Texture.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include <glm/glm.hpp>
#include <memory>

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
				Nebula::NebulaGui::ColorEdit3("Color", &light.Color.x);
				Nebula::NebulaGui::DragFloat("Intensity", &light.Intensity, 0.1f, 0.0f, 100.0f);
				Nebula::NebulaGui::DragFloat("Radius", &light.Radius, 0.1f, 0.0f, 100.0f);
			}
		}

		static void DrawDirectionalLightComponent(Nebula::DirectionalLightComponent& light)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Directional Light", true))
			{
				Nebula::NebulaGui::Text("Direction is controlled by entity rotation");
				Nebula::NebulaGui::ColorEdit3("Color", &light.Color.x);
				Nebula::NebulaGui::DragFloat("Intensity", &light.Intensity, 0.1f, 0.0f, 100.0f);
			}
		}

		static void DrawCameraComponent(Nebula::CameraComponent& camera)
		{
			if (Nebula::NebulaGui::CollapsingHeader("Camera", true))
			{
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
				Nebula::NebulaGui::Text("Script: %s", script.ScriptPath.c_str());
			}
		}

	private:
		static Nebula::Entity s_SelectedEntity;
	};

	inline Nebula::Entity PropertiesPanel::s_SelectedEntity = {};
}

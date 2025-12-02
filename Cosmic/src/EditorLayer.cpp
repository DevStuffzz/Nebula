#include "EditorLayer.h"
#include "Nebula/Math.h"
#include "Nebula/Renderer/RenderCommand.h"
#include "Nebula/Log.h"
#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Application.h"
#include <Nebula/Scene/Components.h>
#include <Nebula/Scene/SceneSerializer.h>

#include "Nebula/Renderer/Mesh.h"
#include <Nebula/Renderer/Shader.h>

#include "Nebula/Renderer/Material.h"
#include "Nebula/Renderer/Texture.h"

#include "EditorWindows/Viewport.h"
#include "EditorWindows/SceneHeirarchy.h"
#include "EditorWindows/PropertiesPanel.h"
#include "EditorWindows/MenuBar.h"
#include "EditorWindows/ContentBrowser.h"
#include "EditorWindows/ScriptEditor.h"

#include "Nebula/Input.h"
#include "Nebula/Keycodes.h"
#include "Nebula/MouseButtonCodes.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <filesystem>

#ifdef NB_PLATFORM_WINDOWS
#include <Windows.h>
#include <commdlg.h>
#endif

namespace Cosmic {

	EditorLayer::EditorLayer()
		: Nebula::Layer("EditorLayer"), m_ActiveScene(nullptr), m_ViewportSize(1920.0f, 1080.0f)
	{
	}

	void EditorLayer::OnAttach()
	{
		NB_CORE_INFO("EditorLayer attached");

		// Create framebuffer with initial fixed size (can be resized later)
		Nebula::FramebufferSpecification fbSpec;
		fbSpec.Width = (uint32_t)m_ViewportSize.x;
		fbSpec.Height = (uint32_t)m_ViewportSize.y;
		m_Framebuffer = Nebula::Framebuffer::Create(fbSpec);

		auto& camera = Nebula::Application::Get().GetCamera();
		if (auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&camera))
		{
			perspCam->SetPosition(m_CameraPosition);
			perspCam->SetRotation(m_CameraRotation);
		}

		// Create a default scene if none is set
		if (!m_ActiveScene)
		{
			m_ActiveScene = std::make_shared<Nebula::Scene>("Top-Down Scene");

			// Create ground
			auto ground = m_ActiveScene->CreateEntity("Ground");
			auto& groundMeshRenderer = ground.AddComponent<Nebula::MeshRendererComponent>();
			groundMeshRenderer.Mesh = Nebula::Mesh::CreateCube();
			auto& groundTransform = ground.GetComponent<Nebula::TransformComponent>();
			groundTransform.Scale = glm::vec3(20.0f, 0.1f, 20.0f);
			groundTransform.Position = glm::vec3(0.0f, -0.5f, 0.0f);

			// Create shader and material for ground
			Nebula::Shader* rawShader = Nebula::Shader::Create("assets/shaders/Basic.glsl");
			std::shared_ptr<Nebula::Shader> shader(rawShader);

			auto groundMaterial = std::make_shared<Nebula::Material>(shader);
			groundMaterial->SetFloat4("u_Color", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)); // Gray color

			std::shared_ptr<Nebula::Texture2D> texture;
			texture.reset(Nebula::Texture2D::Create("assets/textures/Checkerboard.png"));
			groundMaterial->SetTexture("u_Texture", texture);
			groundMaterial->SetInt("u_UseTexture", 1);

			groundMeshRenderer.Material = groundMaterial;

			// Create a sample object (e.g., a box)
			auto box = m_ActiveScene->CreateEntity("Box");
			auto& boxMeshRenderer = box.AddComponent<Nebula::MeshRendererComponent>();
			boxMeshRenderer.Mesh = Nebula::Mesh::CreateCube();
			auto& boxTransform = box.GetComponent<Nebula::TransformComponent>();
			boxTransform.Position = glm::vec3(2.0f, 0.5f, 0.0f);
			boxTransform.Scale = glm::vec3(1.0f, 1.0f, 1.0f);

			auto boxMaterial = std::make_shared<Nebula::Material>(shader);
			boxMaterial->SetFloat4("u_Color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red color
			boxMaterial->SetTexture("u_Texture", texture);
			boxMaterial->SetInt("u_UseTexture", 0); // No texture

			boxMeshRenderer.Material = boxMaterial;
		}

		// Create a camera entity
		auto cameraEntity = m_ActiveScene->CreateEntity("Main Camera");
		auto& cameraComp = cameraEntity.AddComponent<Nebula::CameraComponent>();
		auto& cameraTransform = cameraEntity.GetComponent<Nebula::TransformComponent>();
		cameraTransform.Position = glm::vec3(0.0f, 15.0f, 10.0f);
		cameraTransform.Rotation = glm::vec3(-glm::half_pi<float>(), 0.0f, 0.0f); // Look along negative Y (down, since Y is up)

		// Add CameraController script
		auto& scriptComp = cameraEntity.AddComponent<Nebula::ScriptComponent>();
		scriptComp.ScriptPath = "assets/scripts/CameraController.lua";

		SceneHierarchy::SetContext(m_ActiveScene);

		// Setup content browser
		ContentBrowser::Initialize();
		ContentBrowser::SetContentPath("assets");
		ContentBrowser::SetSceneLoadCallback([this](const std::string& path) {
			LoadSceneFromPath(path);
		});

		// Setup menu bar callbacks
		MenuBar::SetCallbacks(
			[this]() { NewScene(); },
			[this]() { SaveScene(); },
			[this]() { LoadScene(); },
			[]() { /* Exit handled elsewhere */ },
			[this]() { ToggleRuntime(); },
			[]() { ScriptEditor::ToggleVisibility(); }
		);

	}

	void EditorLayer::OnDetach()
	{
		delete m_Framebuffer;

		if (m_ActiveScene)
		{
			m_ActiveScene.reset();
		}
	}

	void EditorLayer::OnUpdate(Nebula::Timestep ts)
	{
	if (!m_RuntimeMode)
	{
		// Handle gizmo dragging
		auto gizmoMode = Viewport::GetGizmoMode();
		Nebula::Entity selectedEntity = SceneHierarchy::GetSelectedEntity();
		
		if (selectedEntity && gizmoMode != Viewport::GizmoMode::None)
		{
			// Use ImGui's mouse position for accurate coordinates
			glm::vec2 currentMousePos = Nebula::NebulaGui::GetMousePos();
			
			// Start dragging (only when viewport is hovered)
			if (Nebula::Input::IsMouseButtonPressed(NB_MOUSE_BUTTON_1) && !m_IsUsingGizmo && Viewport::IsViewportHovered())
			{
				m_ActiveAxis = GetHoveredAxis(currentMousePos);
				if (m_ActiveAxis != GizmoAxis::None)
				{
					m_IsUsingGizmo = true;
					m_MouseStartPos = currentMousePos;
					if (selectedEntity.HasComponent<Nebula::TransformComponent>())
					{
						auto& tc = selectedEntity.GetComponent<Nebula::TransformComponent>();
						m_GizmoStartPos = tc.Position;
						
						// Calculate initial intersection to use as offset
						auto& camera = Nebula::Application::Get().GetCamera();
						auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&camera);
						if (perspCam)
						{
							auto bounds = Viewport::GetViewportBounds();
							if (bounds)
							{
								auto screenToRay = [&](const glm::vec2& mousePos) -> std::pair<glm::vec3, glm::vec3> {
									float viewportWidth = bounds[1].x - bounds[0].x;
									float viewportHeight = bounds[1].y - bounds[0].y;
									
									float ndcX = ((mousePos.x - bounds[0].x) / viewportWidth) * 2.0f - 1.0f;
									float ndcY = 1.0f - ((mousePos.y - bounds[0].y) / viewportHeight) * 2.0f;
									
									glm::mat4 invViewProj = glm::inverse(perspCam->GetProjectionMatrix() * perspCam->GetViewMatrix());
									glm::vec4 rayClipNear = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
									glm::vec4 rayClipFar = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);
									
									glm::vec4 rayWorldNear = invViewProj * rayClipNear;
									glm::vec4 rayWorldFar = invViewProj * rayClipFar;
									
									rayWorldNear /= rayWorldNear.w;
									rayWorldFar /= rayWorldFar.w;
									
									glm::vec3 rayOrigin = glm::vec3(rayWorldNear);
									glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorldFar) - glm::vec3(rayWorldNear));
									
									return {rayOrigin, rayDir};
								};
								
								auto rayPlaneIntersect = [](const glm::vec3& rayOrigin, const glm::vec3& rayDir,
															const glm::vec3& planePoint, const glm::vec3& planeNormal) -> glm::vec3 {
									float denom = glm::dot(planeNormal, rayDir);
									if (glm::abs(denom) > 0.0001f) {
										float t = glm::dot(planePoint - rayOrigin, planeNormal) / denom;
										if (t >= 0) {
											return rayOrigin + rayDir * t;
										}
									}
									return rayOrigin;
								};
								
								auto [rayOrigin, rayDir] = screenToRay(currentMousePos);
								
								// Calculate initial intersection based on active axis
								if (m_ActiveAxis == GizmoAxis::X || m_ActiveAxis == GizmoAxis::Y || m_ActiveAxis == GizmoAxis::Z)
								{
									glm::vec3 axis(0.0f);
									if (m_ActiveAxis == GizmoAxis::X) axis = glm::vec3(1, 0, 0);
									else if (m_ActiveAxis == GizmoAxis::Y) axis = glm::vec3(0, 1, 0);
									else if (m_ActiveAxis == GizmoAxis::Z) axis = glm::vec3(0, 0, 1);
									
									glm::vec3 cameraRight = glm::normalize(glm::cross(rayDir, axis));
									glm::vec3 planeNormal = glm::normalize(glm::cross(axis, cameraRight));
									glm::vec3 intersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, planeNormal);
									
									glm::vec3 toIntersection = intersection - m_GizmoStartPos;
									float projection = glm::dot(toIntersection, axis);
									m_DragStartIntersection = m_GizmoStartPos + axis * projection;
								}
								else if (m_ActiveAxis == GizmoAxis::XY)
								{
									glm::vec3 planeNormal(0, 0, 1);
									m_DragStartIntersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, planeNormal);
								}
								else if (m_ActiveAxis == GizmoAxis::XZ)
								{
									glm::vec3 planeNormal(0, 1, 0);
									m_DragStartIntersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, planeNormal);
								}
								else if (m_ActiveAxis == GizmoAxis::YZ)
								{
									glm::vec3 planeNormal(1, 0, 0);
									m_DragStartIntersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, planeNormal);
								}
							}
						}
					}
				}
			}
			
			// Update during drag (continue even if not hovering viewport)
			if (m_IsUsingGizmo && Nebula::Input::IsMouseButtonPressed(NB_MOUSE_BUTTON_1))
			{
				if (selectedEntity.HasComponent<Nebula::TransformComponent>())
				{
					auto& tc = selectedEntity.GetComponent<Nebula::TransformComponent>();
					
					// Get camera for ray casting
					auto& camera = Nebula::Application::Get().GetCamera();
					auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&camera);
					if (!perspCam) return;
					
					auto bounds = Viewport::GetViewportBounds();
					if (!bounds) return;
					
				// Helper to cast ray from mouse position
				auto screenToRay = [&](const glm::vec2& mousePos) -> std::pair<glm::vec3, glm::vec3> {
					float viewportWidth = bounds[1].x - bounds[0].x;
					float viewportHeight = bounds[1].y - bounds[0].y;
					
					float ndcX = ((mousePos.x - bounds[0].x) / viewportWidth) * 2.0f - 1.0f;
					float ndcY = 1.0f - ((mousePos.y - bounds[0].y) / viewportHeight) * 2.0f;						glm::mat4 invViewProj = glm::inverse(perspCam->GetProjectionMatrix() * perspCam->GetViewMatrix());
						glm::vec4 rayClipNear = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
						glm::vec4 rayClipFar = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);
						
						glm::vec4 rayWorldNear = invViewProj * rayClipNear;
						glm::vec4 rayWorldFar = invViewProj * rayClipFar;
						
						rayWorldNear /= rayWorldNear.w;
						rayWorldFar /= rayWorldFar.w;
						
						glm::vec3 rayOrigin = glm::vec3(rayWorldNear);
						glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorldFar) - glm::vec3(rayWorldNear));
						
						return {rayOrigin, rayDir};
					};
					
					// Helper to intersect ray with plane
					auto rayPlaneIntersect = [](const glm::vec3& rayOrigin, const glm::vec3& rayDir,
												const glm::vec3& planePoint, const glm::vec3& planeNormal) -> glm::vec3 {
						float denom = glm::dot(planeNormal, rayDir);
						if (glm::abs(denom) > 0.0001f) {
							float t = glm::dot(planePoint - rayOrigin, planeNormal) / denom;
							if (t >= 0) {
								return rayOrigin + rayDir * t;
							}
						}
						return rayOrigin; // Fallback
					};
					
					auto [rayOrigin, rayDir] = screenToRay(currentMousePos);
					
					if (gizmoMode == Viewport::GizmoMode::Translate)
					{
						glm::vec3 currentIntersection = m_GizmoStartPos;
						
						// Single axis movement - project to axis line
						if (m_ActiveAxis == GizmoAxis::X || m_ActiveAxis == GizmoAxis::Y || m_ActiveAxis == GizmoAxis::Z)
						{
							glm::vec3 axis(0.0f);
							if (m_ActiveAxis == GizmoAxis::X) axis = glm::vec3(1, 0, 0);
							else if (m_ActiveAxis == GizmoAxis::Y) axis = glm::vec3(0, 1, 0);
							else if (m_ActiveAxis == GizmoAxis::Z) axis = glm::vec3(0, 0, 1);
							
							// Find closest point on axis line to ray
							glm::vec3 cameraRight = glm::normalize(glm::cross(rayDir, axis));
							glm::vec3 planeNormal = glm::normalize(glm::cross(axis, cameraRight));
							glm::vec3 intersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, planeNormal);
							
							// Project intersection onto axis
							glm::vec3 toIntersection = intersection - m_GizmoStartPos;
							float projection = glm::dot(toIntersection, axis);
							currentIntersection = m_GizmoStartPos + axis * projection;
						}
						// Plane movement - intersect with actual plane
						else if (m_ActiveAxis == GizmoAxis::XY)
						{
							glm::vec3 planeNormal(0, 0, 1); // XY plane faces Z
							currentIntersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, planeNormal);
							currentIntersection.z = m_GizmoStartPos.z; // Constrain to XY plane
						}
						else if (m_ActiveAxis == GizmoAxis::XZ)
						{
							glm::vec3 planeNormal(0, 1, 0); // XZ plane faces Y
							currentIntersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, planeNormal);
							currentIntersection.y = m_GizmoStartPos.y; // Constrain to XZ plane
						}
						else if (m_ActiveAxis == GizmoAxis::YZ)
						{
							glm::vec3 planeNormal(1, 0, 0); // YZ plane faces X
							currentIntersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, planeNormal);
							currentIntersection.x = m_GizmoStartPos.x; // Constrain to YZ plane
						}
						
						// Apply offset from initial drag point
						glm::vec3 delta = currentIntersection - m_DragStartIntersection;
						tc.Position = m_GizmoStartPos + delta;
					}
				}
			}
			
			// End dragging
			if (!Nebula::Input::IsMouseButtonPressed(NB_MOUSE_BUTTON_1))
			{
				m_IsUsingGizmo = false;
				m_ActiveAxis = GizmoAxis::None;
			}
		}			float moveSpeed = 2.5f * ts;
			
			// Speed up camera when shift is held
			if (Nebula::Input::IsKeyPressed(NB_KEY_LEFT_SHIFT) || Nebula::Input::IsKeyPressed(NB_KEY_RIGHT_SHIFT))
				moveSpeed *= 3.0f;

			// Check if right mouse button held for camera rotation (only if not using gizmo)
			if (Nebula::Input::IsMouseButtonPressed(NB_MOUSE_BUTTON_2) && !m_IsUsingGizmo)
			{
				Nebula::Application::Get().GetWindow().SetCursorMode(true); // Lock cursor

				auto [mouseX, mouseY] = Nebula::Input::GetMousePos();

				if (m_FirstMouse)
				{
					m_LastMouseX = mouseX;
					m_LastMouseY = mouseY;
					m_FirstMouse = false;
				}

				float xOffset = mouseX - m_LastMouseX;
				float yOffset = m_LastMouseY - mouseY; // Y inverted

				m_LastMouseX = mouseX;
				m_LastMouseY = mouseY;

				float sensitivity = 0.1f;
				xOffset *= sensitivity;
				yOffset *= sensitivity;

				m_CameraRotation.y -= xOffset; // yaw
				m_CameraRotation.x += yOffset; // pitch

				// Clamp pitch
				if (m_CameraRotation.x > 89.0f) m_CameraRotation.x = 89.0f;
				if (m_CameraRotation.x < -89.0f) m_CameraRotation.x = -89.0f;

				// Calculate direction vectors based on updated rotation
			float pitch = glm::radians(m_CameraRotation.x);
			float yaw = glm::radians(m_CameraRotation.y);

			glm::mat4 rotation(1.0f);
			rotation = glm::rotate(rotation, yaw, glm::vec3(0, 1, 0));   // yaw
			rotation = glm::rotate(rotation, pitch, glm::vec3(1, 0, 0)); // pitch

			glm::vec3 forward = -glm::vec3(rotation[2]);
			glm::vec3 right = glm::vec3(rotation[0]);

			// Movement input - WASD always available when right-clicking
			if (Nebula::Input::IsKeyPressed(NB_KEY_W))
				m_CameraPosition += forward * moveSpeed;
			if (Nebula::Input::IsKeyPressed(NB_KEY_S))
				m_CameraPosition -= forward * moveSpeed;
			if (Nebula::Input::IsKeyPressed(NB_KEY_A))
				m_CameraPosition -= right * moveSpeed;
			if (Nebula::Input::IsKeyPressed(NB_KEY_D))
				m_CameraPosition += right * moveSpeed;

			// Q for up, E for down
			if (Nebula::Input::IsKeyPressed(NB_KEY_Q))
				m_CameraPosition.y += moveSpeed;
			if (Nebula::Input::IsKeyPressed(NB_KEY_E))
				m_CameraPosition.y -= moveSpeed;

			}
			else
			{
				Nebula::Application::Get().GetWindow().SetCursorMode(false); // Unlock cursor
				m_FirstMouse = true;
			}

			
			// Update camera transform
			auto& camera = Nebula::Application::Get().GetCamera();
			if (auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&camera))
			{
				perspCam->SetPosition(m_CameraPosition);
				perspCam->SetRotation(m_CameraRotation);
			}
		}

		// Update camera from scene if in runtime
		if (m_RuntimeMode && m_ActiveScene)
		{
			auto view = m_ActiveScene->GetRegistry().view<Nebula::CameraComponent, Nebula::TransformComponent>();
			for (auto entity : view)
			{
				auto& cameraComp = view.get<Nebula::CameraComponent>(entity);
				if (cameraComp.Primary)
				{
					auto& transform = view.get<Nebula::TransformComponent>(entity);
					auto& appCamera = Nebula::Application::Get().GetCamera();
					if (auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&appCamera))
					{
						perspCam->SetPosition(transform.Position);
						perspCam->SetRotation(transform.Rotation);
						float aspect = m_ViewportSize.x / m_ViewportSize.y;
						perspCam->SetProjection(cameraComp.PerspectiveFOV, aspect, cameraComp.PerspectiveNear, cameraComp.PerspectiveFar);
					}
					break;
				}
			}
		}

		// --- Existing framebuffer bind, render code ---
		m_Framebuffer->Bind();
		Nebula::RenderCommand::SetViewport(0, 0, (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		Nebula::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Nebula::RenderCommand::Clear();

		if (m_ActiveScene)
		{
			m_ActiveScene->OnUpdate(ts);
			m_ActiveScene->OnRender();
		}

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		// Menu Bar
		MenuBar::OnImGuiRender();

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();

		// Render the viewport with gizmo callback
		auto gizmoCallback = [this]() {
			RenderGizmo();
		};

		glm::vec2 actualDisplaySize = Viewport::OnImGuiRender((void*)(intptr_t)textureID, m_ViewportSize, 
														   glm::vec2{ 0,1 }, glm::vec2{ 1,0 }, gizmoCallback);

		// Clamp minimum size to avoid zero-sized framebuffer
		if (actualDisplaySize.x < 1.0f) actualDisplaySize.x = 1.0f;
		if (actualDisplaySize.y < 1.0f) actualDisplaySize.y = 1.0f;

		// Resize framebuffer only if size changed - match the actual displayed image size
		if ((uint32_t)actualDisplaySize.x != (uint32_t)m_ViewportSize.x ||
			(uint32_t)actualDisplaySize.y != (uint32_t)m_ViewportSize.y)
		{
			m_ViewportSize = actualDisplaySize;
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Scene Hierarchy
		SceneHierarchy::OnImGuiRender();

		// Properties Panel - sync selection from hierarchy
		PropertiesPanel::SetSelectedEntity(SceneHierarchy::GetSelectedEntity());
		PropertiesPanel::OnImGuiRender();

		// Script Editor
		ScriptEditor::OnImGuiRender();

		// Content Browser
		ContentBrowser::OnImGuiRender();
	}

	void EditorLayer::OnEvent(Nebula::Event& e)
	{
		// Event handling if needed
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = std::make_shared<Nebula::Scene>("Untitled Scene");
		m_CurrentScenePath.clear();
		SceneHierarchy::SetContext(m_ActiveScene);
		NB_CORE_INFO("Created new scene");
	}

	void EditorLayer::SaveScene()
	{
		if (m_CurrentScenePath.empty())
		{
#ifdef NB_PLATFORM_WINDOWS
			// Open save file dialog on Windows
			OPENFILENAMEA ofn;
			CHAR szFile[260] = { 0 };
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "Nebula Scene\0*.nebscene\0All Files\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = "assets\\scenes";
			ofn.lpstrDefExt = "nebscene";
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

			if (GetSaveFileNameA(&ofn) == TRUE)
			{
				m_CurrentScenePath = ofn.lpstrFile;
			}
			else
			{
				// User cancelled - don't save
				return;
			}
#else
			// Fallback for other platforms
			std::filesystem::create_directories("assets/scenes");
			m_CurrentScenePath = "assets/scenes/" + m_ActiveScene->GetName() + ".nebscene";
#endif
		}

		Nebula::SceneSerializer serializer(m_ActiveScene.get());
		if (serializer.Serialize(m_CurrentScenePath))
		{
			NB_CORE_INFO("Scene saved to: {0}", m_CurrentScenePath);
		}
		else
		{
			NB_CORE_ERROR("Failed to save scene!");
		}
	}

	void EditorLayer::LoadScene()
	{
#ifdef NB_PLATFORM_WINDOWS
		// Open file dialog on Windows
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "Nebula Scene\0*.nebscene\0All Files\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = "assets\\scenes";
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			LoadSceneFromPath(ofn.lpstrFile);
		}
#else
		// Fallback for other platforms - use default path
		LoadSceneFromPath("assets/scenes/Untitled Scene.nebscene");
#endif
	}

	void EditorLayer::LoadSceneFromPath(const std::string& filepath)
	{
		if (!std::filesystem::exists(filepath))
		{
			NB_CORE_WARN("Scene file not found: {0}", filepath);
			return;
		}

		if (!m_ActiveScene)
		{
			m_ActiveScene = std::make_shared<Nebula::Scene>();
		}

		Nebula::SceneSerializer serializer(m_ActiveScene.get());
		if (serializer.Deserialize(filepath))
		{
			m_CurrentScenePath = filepath;
			SceneHierarchy::SetContext(m_ActiveScene);
			NB_CORE_INFO("Scene loaded from: {0}", filepath);
		}
		else
		{
			NB_CORE_ERROR("Failed to load scene!");
		}
	}

	void EditorLayer::ToggleRuntime()
	{
		m_RuntimeMode = !m_RuntimeMode;
		MenuBar::SetRuntimeMode(m_RuntimeMode);
		if (m_RuntimeMode)
		{
			NB_CORE_INFO("Runtime started");
		}
		else
		{
			NB_CORE_INFO("Runtime stopped");
		}
	}

	void EditorLayer::RenderGizmo()
	{
		Nebula::Entity selectedEntity = SceneHierarchy::GetSelectedEntity();
		
		// Get gizmo mode from viewport toolbar
		auto gizmoMode = Viewport::GetGizmoMode();
		
		// Early exit if conditions not met
		if (!selectedEntity || gizmoMode == Viewport::GizmoMode::None || m_RuntimeMode)
			return;
			
		if (!selectedEntity.GetScene())
			return;
			
		if (!selectedEntity.HasComponent<Nebula::TransformComponent>())
			return;
		
		auto& tc = selectedEntity.GetComponent<Nebula::TransformComponent>();
		glm::vec3 origin = tc.Position;
		
		// Get camera for projection
		auto& camera = Nebula::Application::Get().GetCamera();
		auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&camera);
		if (!perspCam) return;
		
	glm::mat4 viewProj = perspCam->GetProjectionMatrix() * perspCam->GetViewMatrix();
	
	auto bounds = Viewport::GetViewportBounds();
	if (!bounds)
		return;
		
	// Validate viewport bounds
	float viewportWidth = bounds[1].x - bounds[0].x;
	float viewportHeight = bounds[1].y - bounds[0].y;
	if (viewportWidth <= 0.0f || viewportHeight <= 0.0f)
		return;
	
	// Draw list for rendering - use window draw list to clip to viewport
	void* drawListPtr = Nebula::NebulaGui::GetWindowDrawList();
	if (!drawListPtr)
		return;
	
	// Cast to ImDrawList for drawing operations
	ImDrawList* drawList = static_cast<ImDrawList*>(drawListPtr);
	
	// Clip drawing to viewport bounds
	drawList->PushClipRect(ImVec2(bounds[0].x, bounds[0].y), ImVec2(bounds[1].x, bounds[1].y), true);
	
	float axisLength = 1.0f;
	float lineThickness = 3.0f;
	
	// Get hovered axis for highlighting
	auto [mouseX, mouseY] = Nebula::Input::GetMousePos();
	GizmoAxis hoveredAxis = m_IsUsingGizmo ? m_ActiveAxis : GetHoveredAxis(glm::vec2(mouseX, mouseY));
	
	// Colors for axes (highlight if hovered or active)
	ImU32 xColor = (hoveredAxis == GizmoAxis::X) ? IM_COL32(255, 255, 0, 255) : IM_COL32(255, 0, 0, 255);   // Yellow when active, Red otherwise
	ImU32 yColor = (hoveredAxis == GizmoAxis::Y) ? IM_COL32(255, 255, 0, 255) : IM_COL32(0, 255, 0, 255);   // Yellow when active, Green otherwise
	ImU32 zColor = (hoveredAxis == GizmoAxis::Z) ? IM_COL32(255, 255, 0, 255) : IM_COL32(0, 0, 255, 255);   // Yellow when active, Blue otherwise		// Function to project 3D point to 2D screen space
		auto project = [&](const glm::vec3& worldPos) -> glm::vec2 {
			glm::vec4 clip = viewProj * glm::vec4(worldPos, 1.0f);
			if (clip.w == 0.0f) clip.w = 0.001f;
			glm::vec3 ndc = glm::vec3(clip) / clip.w;
			
			float viewportWidth = bounds[1].x - bounds[0].x;
			float viewportHeight = bounds[1].y - bounds[0].y;
			
			float x = bounds[0].x + (ndc.x + 1.0f) * 0.5f * viewportWidth;
			float y = bounds[0].y + (1.0f - ndc.y) * 0.5f * viewportHeight;
			
			return glm::vec2(x, y);
		};
		
		// Project origin and axis endpoints
		glm::vec2 originScreen = project(origin);
		glm::vec2 xScreen = project(origin + glm::vec3(axisLength, 0, 0));
		glm::vec2 yScreen = project(origin + glm::vec3(0, axisLength, 0));
		glm::vec2 zScreen = project(origin + glm::vec3(0, 0, axisLength));
		
	// Draw axes based on operation type
	if (gizmoMode == Viewport::GizmoMode::Translate)
	{
		float planeSize = 0.25f; // Size of plane dragging squares in world units
		float planeOffset = 0.3f; // Position along axis (0-1)
		
		// Draw X axis (red)
		drawList->AddLine(ImVec2(originScreen.x, originScreen.y), 
						  ImVec2(xScreen.x, xScreen.y), 
						  xColor, lineThickness);
		drawList->AddCircleFilled(ImVec2(xScreen.x, xScreen.y), 6.0f, xColor);
		
		// Draw Y axis (green)
		drawList->AddLine(ImVec2(originScreen.x, originScreen.y), 
						  ImVec2(yScreen.x, yScreen.y), 
						  yColor, lineThickness);
		drawList->AddCircleFilled(ImVec2(yScreen.x, yScreen.y), 6.0f, yColor);
		
		// Draw Z axis (blue)
		drawList->AddLine(ImVec2(originScreen.x, originScreen.y), 
						  ImVec2(zScreen.x, zScreen.y), 
						  zColor, lineThickness);
		drawList->AddCircleFilled(ImVec2(zScreen.x, zScreen.y), 6.0f, zColor);
		
		// Determine colors based on hover/active state
		ImU32 xyColor = (hoveredAxis == GizmoAxis::XY) ? IM_COL32(255, 255, 0, 180) : IM_COL32(255, 255, 0, 100);
		ImU32 xzColor = (hoveredAxis == GizmoAxis::XZ) ? IM_COL32(255, 0, 255, 180) : IM_COL32(255, 0, 255, 100);
		ImU32 yzColor = (hoveredAxis == GizmoAxis::YZ) ? IM_COL32(0, 255, 255, 180) : IM_COL32(0, 255, 255, 100);
		
		// Draw plane dragging squares as actual planes facing the correct direction
		// XY plane (yellow) - square on the XY plane, facing Z direction
		{
			glm::vec3 center = origin + glm::vec3(planeOffset, planeOffset, 0) * axisLength;
			glm::vec3 p1 = center + glm::vec3(-planeSize, -planeSize, 0) * 0.5f;
			glm::vec3 p2 = center + glm::vec3(planeSize, -planeSize, 0) * 0.5f;
			glm::vec3 p3 = center + glm::vec3(planeSize, planeSize, 0) * 0.5f;
			glm::vec3 p4 = center + glm::vec3(-planeSize, planeSize, 0) * 0.5f;
			
			glm::vec2 s1 = project(p1);
			glm::vec2 s2 = project(p2);
			glm::vec2 s3 = project(p3);
			glm::vec2 s4 = project(p4);
			
			drawList->AddQuadFilled(
				ImVec2(s1.x, s1.y), ImVec2(s2.x, s2.y),
				ImVec2(s3.x, s3.y), ImVec2(s4.x, s4.y),
				xyColor
			);
		}
		
		// XZ plane (magenta) - square on the XZ plane, facing Y direction
		{
			glm::vec3 center = origin + glm::vec3(planeOffset, 0, planeOffset) * axisLength;
			glm::vec3 p1 = center + glm::vec3(-planeSize, 0, -planeSize) * 0.5f;
			glm::vec3 p2 = center + glm::vec3(planeSize, 0, -planeSize) * 0.5f;
			glm::vec3 p3 = center + glm::vec3(planeSize, 0, planeSize) * 0.5f;
			glm::vec3 p4 = center + glm::vec3(-planeSize, 0, planeSize) * 0.5f;
			
			glm::vec2 s1 = project(p1);
			glm::vec2 s2 = project(p2);
			glm::vec2 s3 = project(p3);
			glm::vec2 s4 = project(p4);
			
			drawList->AddQuadFilled(
				ImVec2(s1.x, s1.y), ImVec2(s2.x, s2.y),
				ImVec2(s3.x, s3.y), ImVec2(s4.x, s4.y),
				xzColor
			);
		}
		
		// YZ plane (cyan) - square on the YZ plane, facing X direction
		{
			glm::vec3 center = origin + glm::vec3(0, planeOffset, planeOffset) * axisLength;
			glm::vec3 p1 = center + glm::vec3(0, -planeSize, -planeSize) * 0.5f;
			glm::vec3 p2 = center + glm::vec3(0, planeSize, -planeSize) * 0.5f;
			glm::vec3 p3 = center + glm::vec3(0, planeSize, planeSize) * 0.5f;
			glm::vec3 p4 = center + glm::vec3(0, -planeSize, planeSize) * 0.5f;
			
			glm::vec2 s1 = project(p1);
			glm::vec2 s2 = project(p2);
			glm::vec2 s3 = project(p3);
			glm::vec2 s4 = project(p4);
			
			drawList->AddQuadFilled(
				ImVec2(s1.x, s1.y), ImVec2(s2.x, s2.y),
				ImVec2(s3.x, s3.y), ImVec2(s4.x, s4.y),
				yzColor
			);
		}
	}	// Restore clip rect
	drawList->PopClipRect();
}	EditorLayer::GizmoAxis EditorLayer::GetHoveredAxis(const glm::vec2& mousePos)
	{
		Nebula::Entity selectedEntity = SceneHierarchy::GetSelectedEntity();
		if (!selectedEntity || !selectedEntity.HasComponent<Nebula::TransformComponent>())
			return GizmoAxis::None;
		
		auto& tc = selectedEntity.GetComponent<Nebula::TransformComponent>();
		glm::vec3 origin = tc.Position;
		
		// Get camera for projection
		auto& camera = Nebula::Application::Get().GetCamera();
		auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&camera);
		if (!perspCam) return GizmoAxis::None;
		
		glm::mat4 viewProj = perspCam->GetProjectionMatrix() * perspCam->GetViewMatrix();
		auto bounds = Viewport::GetViewportBounds();
		if (!bounds) return GizmoAxis::None;
		
		float axisLength = 1.0f;
		float hitRadius = 15.0f; // Pixels
		
		// Function to project 3D point to 2D screen space
		auto project = [&](const glm::vec3& worldPos) -> glm::vec2 {
			glm::vec4 clip = viewProj * glm::vec4(worldPos, 1.0f);
			if (clip.w == 0.0f) clip.w = 0.001f;
			glm::vec3 ndc = glm::vec3(clip) / clip.w;
			
			float viewportWidth = bounds[1].x - bounds[0].x;
			float viewportHeight = bounds[1].y - bounds[0].y;
			
			float x = bounds[0].x + (ndc.x + 1.0f) * 0.5f * viewportWidth;
			float y = bounds[0].y + (1.0f - ndc.y) * 0.5f * viewportHeight;
			
			return glm::vec2(x, y);
		};
		
	// Project axis endpoints
	glm::vec2 originScreen = project(origin);
	glm::vec2 xScreen = project(origin + glm::vec3(axisLength, 0, 0));
	glm::vec2 yScreen = project(origin + glm::vec3(0, axisLength, 0));
	glm::vec2 zScreen = project(origin + glm::vec3(0, 0, axisLength));
	
	// Helper function to calculate distance from point to line segment
	auto distanceToLineSegment = [](const glm::vec2& p, const glm::vec2& a, const glm::vec2& b) -> float {
		glm::vec2 ab = b - a;
		glm::vec2 ap = p - a;
		float abLenSq = glm::dot(ab, ab);
		if (abLenSq == 0.0f) return glm::length(ap);
		
		float t = glm::clamp(glm::dot(ap, ab) / abLenSq, 0.0f, 1.0f);
		glm::vec2 projection = a + t * ab;
		return glm::distance(p, projection);
	};
	
	// Helper function to check if point is inside a square
	auto isInsideSquare = [](const glm::vec2& p, const glm::vec2& center, float size) -> bool {
		float halfSize = size * 0.5f;
		return (p.x >= center.x - halfSize && p.x <= center.x + halfSize &&
				p.y >= center.y - halfSize && p.y <= center.y + halfSize);
	};
	
	// Helper function to check if point is inside a quad
	auto isInsideQuad = [](const glm::vec2& p, const glm::vec2& p1, const glm::vec2& p2, 
						   const glm::vec2& p3, const glm::vec2& p4) -> bool {
		// Use cross product to check if point is on the same side of all edges
		auto sign = [](const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) -> float {
			return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
		};
		
		float d1 = sign(p, p1, p2);
		float d2 = sign(p, p2, p3);
		float d3 = sign(p, p3, p4);
		float d4 = sign(p, p4, p1);
		
		bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0) || (d4 < 0);
		bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0) || (d4 > 0);
		
		return !(hasNeg && hasPos);
	};
	
	float planeSize = 0.25f;
	float planeOffset = 0.3f;
	
	// Calculate plane quad vertices (same as in rendering)
	// XY plane
	{
		glm::vec3 center = origin + glm::vec3(planeOffset, planeOffset, 0) * axisLength;
		glm::vec3 p1 = center + glm::vec3(-planeSize, -planeSize, 0) * 0.5f;
		glm::vec3 p2 = center + glm::vec3(planeSize, -planeSize, 0) * 0.5f;
		glm::vec3 p3 = center + glm::vec3(planeSize, planeSize, 0) * 0.5f;
		glm::vec3 p4 = center + glm::vec3(-planeSize, planeSize, 0) * 0.5f;
		
		glm::vec2 s1 = project(p1);
		glm::vec2 s2 = project(p2);
		glm::vec2 s3 = project(p3);
		glm::vec2 s4 = project(p4);
		
		if (isInsideQuad(mousePos, s1, s2, s3, s4))
			return GizmoAxis::XY;
	}
	
	// XZ plane
	{
		glm::vec3 center = origin + glm::vec3(planeOffset, 0, planeOffset) * axisLength;
		glm::vec3 p1 = center + glm::vec3(-planeSize, 0, -planeSize) * 0.5f;
		glm::vec3 p2 = center + glm::vec3(planeSize, 0, -planeSize) * 0.5f;
		glm::vec3 p3 = center + glm::vec3(planeSize, 0, planeSize) * 0.5f;
		glm::vec3 p4 = center + glm::vec3(-planeSize, 0, planeSize) * 0.5f;
		
		glm::vec2 s1 = project(p1);
		glm::vec2 s2 = project(p2);
		glm::vec2 s3 = project(p3);
		glm::vec2 s4 = project(p4);
		
		if (isInsideQuad(mousePos, s1, s2, s3, s4))
			return GizmoAxis::XZ;
	}
	
	// YZ plane
	{
		glm::vec3 center = origin + glm::vec3(0, planeOffset, planeOffset) * axisLength;
		glm::vec3 p1 = center + glm::vec3(0, -planeSize, -planeSize) * 0.5f;
		glm::vec3 p2 = center + glm::vec3(0, planeSize, -planeSize) * 0.5f;
		glm::vec3 p3 = center + glm::vec3(0, planeSize, planeSize) * 0.5f;
		glm::vec3 p4 = center + glm::vec3(0, -planeSize, planeSize) * 0.5f;
		
		glm::vec2 s1 = project(p1);
		glm::vec2 s2 = project(p2);
		glm::vec2 s3 = project(p3);
		glm::vec2 s4 = project(p4);
		
		if (isInsideQuad(mousePos, s1, s2, s3, s4))
			return GizmoAxis::YZ;
	}
	
	// Check distance to each axis line (prioritize Z, then Y, then X for overlap)
	if (distanceToLineSegment(mousePos, originScreen, zScreen) < hitRadius)
		return GizmoAxis::Z;
	if (distanceToLineSegment(mousePos, originScreen, yScreen) < hitRadius)
		return GizmoAxis::Y;
	if (distanceToLineSegment(mousePos, originScreen, xScreen) < hitRadius)
		return GizmoAxis::X;		return GizmoAxis::None;
	}

}


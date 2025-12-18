#include "EditorLayer.h"
#include "Nebula/Math.h"
#include "Nebula/Renderer/RenderCommand.h"
#include "Nebula/Log.h"
#include "Nebula/ImGui/NebulaGuizmo.h"
#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Application.h"
#include <Nebula/Scene/Components.h>
#include <Nebula/Scene/SceneSerializer.h>
#include <Nebula/Scene/SceneManager.h>
#include <Nebula/Physics/PhysicsWorld.h>

#include "Nebula/Renderer/Mesh.h"
#include <Nebula/Renderer/Shader.h>

#include "Nebula/Renderer/Material.h"
#include "Nebula/Renderer/Texture.h"

#include "EditorWindows/Viewport.h"
#include "EditorWindows/GameView.h"
#include "EditorWindows/SceneHeirarchy.h"
#include "EditorWindows/PropertiesPanel.h"
#include "EditorWindows/MenuBar.h"
#include "EditorWindows/ContentBrowser.h"
#include "EditorWindows/ScriptEditor.h"

#include "Nebula/Input.h"
#include "Nebula/Keycodes.h"
#include "Nebula/MouseButtonCodes.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
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

		// Load the scene list at startup
		Nebula::SceneManager::Get().LoadSceneList();

		// Create framebuffer with initial fixed size (can be resized later)
		Nebula::FramebufferSpecification fbSpec;
		fbSpec.Width = (uint32_t)m_ViewportSize.x;
		fbSpec.Height = (uint32_t)m_ViewportSize.y;
		m_Framebuffer = Nebula::Framebuffer::Create(fbSpec);

		// Create game view framebuffer
		Nebula::FramebufferSpecification gameViewSpec;
		gameViewSpec.Width = 1280;
		gameViewSpec.Height = 720;
		m_GameViewFramebuffer = Nebula::Framebuffer::Create(gameViewSpec);
		m_GameViewSize = { 1280.0f, 720.0f };

		// Create line renderer for debug visualization
		m_LineRenderer = Nebula::LineRenderer::Create();

		auto& camera = Nebula::Application::Get().GetCamera();
		if (auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&camera))
		{
			perspCam->SetPosition(m_CameraPosition);
			perspCam->SetRotation(m_CameraRotation);
		}

		// Create a default scene if none is set
		if (!m_ActiveScene)
		{
			m_ActiveScene = std::make_shared<Nebula::Scene>("Default Scene");

			
		// Create a camera entity
		auto cameraEntity = m_ActiveScene->CreateEntity("Main Camera");
		auto& cameraComp = cameraEntity.AddComponent<Nebula::CameraComponent>();
		auto& cameraTransform = cameraEntity.GetComponent<Nebula::TransformComponent>();
		cameraTransform.Position = glm::vec3(0.0f, 15.0f, 10.0f);
		cameraTransform.Rotation = glm::vec3(-glm::half_pi<float>(), 0.0f, 0.0f); // Look along negative Y (down, since Y is up)
		}

		
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
			[]() { ScriptEditor::ToggleVisibility(); },
			[this]() { m_SceneListWindow.SetOpen(true); }
		);

	}

	void EditorLayer::OnDetach()
	{
		delete m_Framebuffer;
		delete m_GameViewFramebuffer;

		if (m_ActiveScene)
		{
			m_ActiveScene.reset();
		}
	}

	void EditorLayer::OnUpdate(Nebula::Timestep ts)
	{
	if (!m_RuntimeMode)
	{
		// Handle gizmo interaction
		auto gizmoMode = Viewport::GetGizmoMode();
		Nebula::Entity selectedEntity = SceneHierarchy::GetSelectedEntity();
		
// Gizmo input handling removed - will be replaced with ImGuizmo

		float moveSpeed = 2.5f * ts;
		
		// Speed up camera when shift is held
		if (Nebula::Input::IsKeyPressed(NB_KEY_LEFT_SHIFT) || Nebula::Input::IsKeyPressed(NB_KEY_RIGHT_SHIFT))
			moveSpeed *= 3.0f;

		// Check if right mouse button held for camera rotation (only if not using gizmo)
		if (Nebula::Input::IsMouseButtonPressed(NB_MOUSE_BUTTON_2) && !Nebula::NebulaGuizmo::IsUsing())
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
			// Only update physics and scripts when in runtime mode
			if (m_RuntimeMode)
			{
				m_ActiveScene->OnUpdate(ts);
			}
			m_ActiveScene->OnRender();
			
			// Always draw physics debug visualization
			if (m_LineRenderer)
			{
				m_ActiveScene->SetPhysicsDebugDraw(true);
				if (m_ActiveScene->GetPhysicsWorld())
				{
					// Sync all entity transforms to physics bodies before debug drawing (only in editor mode)
					if (!m_RuntimeMode)
					{
						m_ActiveScene->GetPhysicsWorld()->SyncAllRigidBodyTransforms(m_ActiveScene.get());
					}
					
					m_ActiveScene->GetPhysicsWorld()->DebugDraw();
					
					// Render physics debug lines
					const auto& lineVertices = m_ActiveScene->GetPhysicsWorld()->GetDebugLineVertices();
					const auto& lineColors = m_ActiveScene->GetPhysicsWorld()->GetDebugLineColors();
					
					if (!lineVertices.empty() && !lineColors.empty())
					{
						// Set view projection from the editor camera
						auto& camera = Nebula::Application::Get().GetCamera();
						m_LineRenderer->SetViewProjection(camera.GetViewProjectionMatrix());
						
						m_LineRenderer->Begin();
						m_LineRenderer->DrawLines(lineVertices, lineColors);
						m_LineRenderer->End();
					}
				}
			}
		}

		m_Framebuffer->Unbind();

		// --- Render Game View ---
		if (m_ActiveScene)
		{
			// Find the primary camera in the scene
			auto view = m_ActiveScene->GetRegistry().view<Nebula::CameraComponent, Nebula::TransformComponent>();
			for (auto entity : view)
			{
				auto& cameraComp = view.get<Nebula::CameraComponent>(entity);
				if (cameraComp.Primary)
				{
					auto& transform = view.get<Nebula::TransformComponent>(entity);
					
					// Bind game view framebuffer
					m_GameViewFramebuffer->Bind();
					Nebula::RenderCommand::SetViewport(0, 0, (uint32_t)m_GameViewSize.x, (uint32_t)m_GameViewSize.y);
					Nebula::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
					Nebula::RenderCommand::Clear();
					
					// Temporarily set the application camera to the game camera
					auto& appCamera = Nebula::Application::Get().GetCamera();
					auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&appCamera);
					if (!perspCam) break;
					
					glm::vec3 savedPos = perspCam->GetPosition();
					glm::vec3 savedRot = perspCam->GetRotation();
					
					perspCam->SetPosition(transform.Position);
					perspCam->SetRotation(transform.Rotation);
					float gameAspect = m_GameViewSize.x / m_GameViewSize.y;
					perspCam->SetProjection(cameraComp.PerspectiveFOV, gameAspect, cameraComp.PerspectiveNear, cameraComp.PerspectiveFar);
					
					// Render scene from game camera perspective
					m_ActiveScene->OnRender();
					
					// Restore application camera
					perspCam->SetPosition(savedPos);
					perspCam->SetRotation(savedRot);
					float editorAspect = m_ViewportSize.x / m_ViewportSize.y;
					perspCam->SetProjection(45.0f, editorAspect, 0.1f, 1000.0f);
					
					m_GameViewFramebuffer->Unbind();
					break;
				}
			}
		}
	}

	void EditorLayer::OnImGuiRender()
	{
	Nebula::NebulaGuizmo::BeginFrame();
	
	// Menu Bar
	MenuBar::OnImGuiRender();

	uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
	auto gizmoCallback = [this]()
	{
		// Render ImGuizmo for selected entity
		Nebula::Entity selectedEntity = SceneHierarchy::GetSelectedEntity();
		auto gizmoMode = Viewport::GetGizmoMode();
		
		if (!selectedEntity || gizmoMode == Viewport::GizmoMode::None || m_RuntimeMode)
			return;
			
		// Get camera from application
		auto& camera = Nebula::Application::Get().GetCamera();
		glm::mat4 cameraView = camera.GetViewMatrix();
		glm::mat4 cameraProjection = camera.GetProjectionMatrix();
		
		// Get entity transform
		auto& tc = selectedEntity.GetComponent<Nebula::TransformComponent>();
		glm::mat4 transform = tc.GetTransform();
		
		// Setup ImGuizmo
		auto bounds = Viewport::GetViewportBounds();
		Nebula::NebulaGuizmo::SetOrthographic(false);
		Nebula::NebulaGuizmo::SetDrawlist();
		Nebula::NebulaGuizmo::SetRect(bounds[0].x, bounds[0].y, 
									  bounds[1].x - bounds[0].x, 
									  bounds[1].y - bounds[0].y);
		
		// Determine operation type
		Nebula::GuizmoOperation operation;
		switch (gizmoMode)
		{
		case Viewport::GizmoMode::Translate: operation = Nebula::GuizmoOperation::Translate; break;
		case Viewport::GizmoMode::Rotate: operation = Nebula::GuizmoOperation::Rotate; break;
		case Viewport::GizmoMode::Scale: operation = Nebula::GuizmoOperation::Scale; break;
		default: return;
		}
		
		// Manipulate the transform
		if (Nebula::NebulaGuizmo::Manipulate(
			&cameraView[0][0],
			&cameraProjection[0][0],
			operation,
			Nebula::GuizmoMode::Local,
			&transform[0][0]))
		{
			// Decompose the transform and update entity
			glm::vec3 translation, rotation, scale;
			Nebula::NebulaGuizmo::DecomposeMatrixToComponents(
				&transform[0][0],
				&translation[0],
				&rotation[0],
				&scale[0]
			);
			
			tc.Position = translation;
			tc.Rotation = rotation;
			tc.Scale = scale;
			
			// Update physics body if entity has one
			if (selectedEntity.HasComponent<Nebula::RigidBodyComponent>() && m_ActiveScene->GetPhysicsWorld())
			{
				m_ActiveScene->GetPhysicsWorld()->UpdateRigidBodyTransform(selectedEntity);
			}
		}
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

		// Game View - separate window showing game camera perspective
		uint32_t gameViewTextureID = m_GameViewFramebuffer->GetColorAttachmentRendererID();
		glm::vec2 actualGameViewSize = GameView::OnImGuiRender((void*)(intptr_t)gameViewTextureID, m_GameViewSize,
															   glm::vec2{ 0,1 }, glm::vec2{ 1,0 });
		
		// Clamp minimum size to avoid zero-sized framebuffer
		if (actualGameViewSize.x < 1.0f) actualGameViewSize.x = 1.0f;
		if (actualGameViewSize.y < 1.0f) actualGameViewSize.y = 1.0f;
		
		// Resize game view framebuffer if size changed
		if ((uint32_t)actualGameViewSize.x != (uint32_t)m_GameViewSize.x ||
			(uint32_t)actualGameViewSize.y != (uint32_t)m_GameViewSize.y)
		{
			m_GameViewSize = actualGameViewSize;
			m_GameViewFramebuffer->Resize((uint32_t)m_GameViewSize.x, (uint32_t)m_GameViewSize.y);
		}

		// Properties Panel - sync selection from hierarchy
		PropertiesPanel::SetSelectedEntity(SceneHierarchy::GetSelectedEntity());
		PropertiesPanel::OnImGuiRender();

		// Script Editor
		ScriptEditor::OnImGuiRender();

		// Content Browser
		ContentBrowser::OnImGuiRender();

		// Scene List Window
		m_SceneListWindow.OnImGuiRender();
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
			// Entering runtime mode - save the current scene state
			NB_CORE_INFO("Runtime started");
			
			// Store the scene path so we can reload it when stopping
			if (!m_CurrentScenePath.empty())
			{
				// Save current editor state before entering play mode
				// This allows us to restore it when exiting play mode
			}
		}
		else
		{
			// Exiting runtime mode - restore the scene to its pre-runtime state
			NB_CORE_INFO("Runtime stopped");
			
			// Reload the scene to reset physics state (this will clear the physics world)
			if (!m_CurrentScenePath.empty())
			{
				LoadSceneFromPath(m_CurrentScenePath);
			}
		}
	}

	// RenderGizmo removed - will be replaced with ImGuizmo integration

}


#include "EditorLayer.h"
#include "Nebula/Math.h"
#include "Nebula/Renderer/RenderCommand.h"
#include "Nebula/Log.h"
#include "Nebula/ImGui/NebulaGuizmo.h"
#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Application.h"
#include "Nebula/Project/Project.h"
#include "Nebula/Scripting/ScriptEngine.h"
#include "Nebula/Scripting/ScriptBuilder.h"
#include <Nebula/Scene/Components.h>
#include <Nebula/Scene/SceneSerializer.h>
#include <Nebula/Scene/SceneManager.h>
#include <Nebula/Physics/PhysicsWorld.h>
#include <Nebula/Audio/AudioEngine.h>

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
#include "Nebula/Core/FileDialog.h"

namespace Cosmic {

	EditorLayer::EditorLayer(const std::string& projectPath)
		: Nebula::Layer("EditorLayer"), m_ActiveScene(nullptr), m_ViewportSize(1920.0f, 1080.0f)
	{
		// If project path provided from launcher, open it immediately
		if (!projectPath.empty())
		{
			NB_CORE_INFO("Opening project from launcher: {0}", projectPath);
			OpenProject(projectPath);
			m_ProjectLoaded = true;
		}
	}

	void EditorLayer::OnAttach()
	{
		NB_CORE_INFO("EditorLayer attached");

		// If project not loaded from launcher, show project browser
		if (!m_ProjectLoaded)
		{
			CheckAndShowProjectBrowser();
		}
		else
		{
			// Project already loaded from launcher, load scene list
			Nebula::SceneManager::Get().LoadSceneList();
		}

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
		// Check for script file changes (only in editor mode)
		if (!m_RuntimeMode && m_ProjectLoaded)
		{
			CheckScriptFileChanges();
		}

		// Handle gizmo interaction (only in editor mode)
		auto gizmoMode = Viewport::GizmoMode::None;
		Nebula::Entity selectedEntity;
		
		if (!m_RuntimeMode)
		{
			gizmoMode = Viewport::GetGizmoMode();
			selectedEntity = SceneHierarchy::GetSelectedEntity();
		}

		// Camera movement (available in both editor and runtime modes)
		float moveSpeed = 2.5f * ts;
		
		// Speed up camera when shift is held
		if (Nebula::Input::IsKeyDown(NB_KEY_LEFT_SHIFT) || Nebula::Input::IsKeyDown(NB_KEY_RIGHT_SHIFT))
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
			if (Nebula::Input::IsKeyDown(NB_KEY_W))
				m_CameraPosition += forward * moveSpeed;
			if (Nebula::Input::IsKeyDown(NB_KEY_S))
				m_CameraPosition -= forward * moveSpeed;
			if (Nebula::Input::IsKeyDown(NB_KEY_A))
				m_CameraPosition -= right * moveSpeed;
			if (Nebula::Input::IsKeyDown(NB_KEY_D))
				m_CameraPosition += right * moveSpeed;

			// Q for up, E for down
			if (Nebula::Input::IsKeyDown(NB_KEY_Q))
				m_CameraPosition.y += moveSpeed;
			if (Nebula::Input::IsKeyDown(NB_KEY_E))
				m_CameraPosition.y -= moveSpeed;

			}
			else
			{
				Nebula::Application::Get().GetWindow().SetCursorMode(false); // Unlock cursor
				m_FirstMouse = true;
			}

		// --- Render Editor Viewport ---
		// Set application camera to editor camera
		auto& editorCamera = Nebula::Application::Get().GetCamera();
		if (auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&editorCamera))
		{
			perspCam->SetPosition(m_CameraPosition);
			perspCam->SetRotation(m_CameraRotation);
			// Guard against zero aspect ratio
			float editorAspect = m_ViewportSize.y > 0.0f ? (m_ViewportSize.x / m_ViewportSize.y) : 16.0f / 9.0f;
			perspCam->SetProjection(45.0f, editorAspect, 0.1f, 1000.0f);
		}
		
		m_Framebuffer->Bind();
		Nebula::RenderCommand::SetViewport(0, 0, (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		Nebula::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Nebula::RenderCommand::Clear();

		if (m_ActiveScene)
		{
			// Only update physics and scripts when in runtime mode
			if (m_RuntimeMode)
			{
				// Process any pending scene loads BEFORE update
				if (Nebula::SceneManager::Get().HasPendingSceneLoad())
				{
					// Just stop audio, don't destroy - let Scene destructor handle cleanup
					if (m_ActiveScene && m_ActiveScene->GetAudioEngine())
					{
						m_ActiveScene->GetAudioEngine()->StopAll();
					}
					
					Nebula::SceneManager::Get().ProcessPendingSceneLoad();
					m_ActiveScene = Nebula::SceneManager::Get().GetActiveScene();
				}
				
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
			// Bind game view framebuffer
			m_GameViewFramebuffer->Bind();
			Nebula::RenderCommand::SetViewport(0, 0, (uint32_t)m_GameViewSize.x, (uint32_t)m_GameViewSize.y);
			
			// Find the primary camera in the scene
			auto view = m_ActiveScene->GetRegistry().view<Nebula::CameraComponent, Nebula::TransformComponent>();
			bool foundPrimaryCamera = false;
			
			for (auto entity : view)
			{
				auto& cameraComp = view.get<Nebula::CameraComponent>(entity);
				if (cameraComp.Primary)
				{
					foundPrimaryCamera = true;
					auto& transform = view.get<Nebula::TransformComponent>(entity);
					
					Nebula::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
					Nebula::RenderCommand::Clear();
					
					// Temporarily set the application camera to the game camera
					auto& appCamera = Nebula::Application::Get().GetCamera();
					auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&appCamera);
					if (!perspCam) break;
					
					// Save complete editor camera state
					glm::vec3 savedPos = perspCam->GetPosition();
					glm::vec3 savedRot = perspCam->GetRotation();
					float savedFOV = 45.0f;
					float savedNear = 0.1f;
					float savedFar = 1000.0f;
				float savedAspect = m_ViewportSize.y > 0.0f ? (m_ViewportSize.x / m_ViewportSize.y) : 16.0f / 9.0f;
				
				// Set to game camera
				perspCam->SetPosition(transform.Position);
				perspCam->SetRotation(transform.Rotation);
				// Guard against zero aspect ratio
				float gameAspect = m_GameViewSize.y > 0.0f ? (m_GameViewSize.x / m_GameViewSize.y) : 16.0f / 9.0f;
				perspCam->SetProjection(cameraComp.PerspectiveFOV, gameAspect, cameraComp.PerspectiveNear, cameraComp.PerspectiveFar);
					
					// Render scene from game camera perspective
					m_ActiveScene->OnRender();
					
					// Restore editor camera completely
					perspCam->SetPosition(savedPos);
					perspCam->SetRotation(savedRot);
					perspCam->SetProjection(savedFOV, savedAspect, savedNear, savedFar);
					
					break;
				}
			}
			
			// If no primary camera found, render error screen (purple)
			if (!foundPrimaryCamera)
			{
				Nebula::RenderCommand::SetClearColor({ 0.5f, 0.0f, 0.5f, 1.0f }); // Purple error
				Nebula::RenderCommand::Clear();
			}
			
			m_GameViewFramebuffer->Unbind();
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

		// Console Window
		ConsoleWindow::OnImGuiRender();

		// Debug Window
		DebugWindow::OnImGuiRender(m_ActiveScene, m_RuntimeMode);

		// Content Browser
		ContentBrowser::OnImGuiRender();

		// Scene List Window
		m_SceneListWindow.OnImGuiRender();

		// Show project browser if needed
		if (m_ShowProjectBrowser)
		{
			if (m_ProjectBrowser.Show(&m_ShowProjectBrowser))
			{
				// Project was selected
				if (m_ProjectBrowser.ShouldCreateNewProject())
				{
					CreateNewProject(m_ProjectBrowser.GetNewProjectName(), m_ProjectBrowser.GetNewProjectPath());
				}
				else
				{
					OpenProject(m_ProjectBrowser.GetSelectedProjectPath());
				}
			}
		}
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
			Nebula::FileDialog* dialog = Nebula::FileDialog::Create();
			auto result = dialog->SaveFile("Nebula Scene\0*.nebscene\0All Files\0*.*\0", "nebscene", "assets/scenes");
			delete dialog;

			if (result.has_value())
			{
				m_CurrentScenePath = result.value();
			}
			else
			{
				// User cancelled - don't save
				return;
			}
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
		Nebula::FileDialog* dialog = Nebula::FileDialog::Create();
		auto result = dialog->OpenFile("Nebula Scene\0*.nebscene\0All Files\0*.*\0", "assets/scenes");
		delete dialog;

		if (result.has_value())
		{
			LoadSceneFromPath(result.value());
		}
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
		if (!m_RuntimeMode)
		{
			// Check if scene is saved
			if (m_CurrentScenePath.empty())
			{
				ConsoleWindow::AddLog("Cannot start runtime: You must save the scene before running.", Nebula::LogLevel::LOG_ERROR);
				NB_CORE_ERROR("Cannot start runtime: Scene must be saved first");
				return;
			}

			// Validate all scripts before entering runtime
			std::string errorMessage;
			if (!m_ActiveScene->ValidateAllScripts(errorMessage))
			{
				ConsoleWindow::AddLog("Cannot start runtime: " + errorMessage, Nebula::LogLevel::LOG_ERROR);
				NB_CORE_ERROR("Cannot start runtime: {0}", errorMessage);
				return;
			}

			// Check for build errors
			if (m_HasBuildErrors)
			{
				std::string buildError = Nebula::ScriptBuilder::GetLastBuildError();
				ConsoleWindow::AddLog("Cannot start runtime: Scripts have build errors", Nebula::LogLevel::LOG_ERROR);
				ConsoleWindow::AddLog(buildError, Nebula::LogLevel::LOG_ERROR);
				NB_CORE_ERROR("Cannot start runtime: Build errors present");
				return;
			}
		}

		m_RuntimeMode = !m_RuntimeMode;
		MenuBar::SetRuntimeMode(m_RuntimeMode);
		ScriptEditor::SetRuntimeMode(m_RuntimeMode);
		
		if (m_RuntimeMode)
		{
			// validate scene first
			if (!m_ActiveScene)
			{
				NB_CORE_ERROR("Cannot start runtime: No active scene");
				ConsoleWindow::AddLog("Cannot start runtime: No active scene", Nebula::LogLevel::LOG_ERROR);
				m_RuntimeMode = false;
				MenuBar::SetRuntimeMode(false);
				return;
			}

			// Check if scene has any entities
			auto entities = m_ActiveScene->GetAllEntities();
			if (entities.empty())
			{
				NB_CORE_WARN("Starting runtime with empty scene");
				ConsoleWindow::AddLog("Warning: Scene has no entities", Nebula::LogLevel::LOG_WARN);
			}

			// Validate entities with script components have valid scripts
			for (auto entity : entities)
			{
				if (entity.HasComponent<Nebula::ScriptComponent>())
				{
					auto& sc = entity.GetComponent<Nebula::ScriptComponent>();
				// TODO: Validate C# class exists
				// if (!sc.ClassName.empty())
				// {
				// 	if (!Nebula::ScriptEngine::EntityClassExists(sc.ClassName))
				// 	{
				// 		auto& tag = entity.GetComponent<Nebula::TagComponent>();
				// 		NB_CORE_ERROR("Entity '{}' references missing C# class: {}", tag.Tag, sc.ClassName);
				// 		ConsoleWindow::AddLog("Error: Entity '" + tag.Tag + "' references missing C# class: " + sc.ClassName, LogLevel::Error);
				// 	}
				// }
			}
		}
		
		// Reset audio sources
		auto audioView = m_ActiveScene->GetRegistry().view<Nebula::AudioSourceComponent>();
		for (auto entity : audioView)
		{
			auto& audioSource = audioView.get<Nebula::AudioSourceComponent>(entity);
				audioSource.RuntimeSourceID = 0;
				audioSource.IsPlaying = false;
			}

			// Initialize script runtime
			m_ActiveScene->OnRuntimeStart();

			NB_CORE_INFO("Runtime started");
			ConsoleWindow::AddLog("Runtime started", Nebula::LogLevel::LOG_INFO);
			
			// Log script entities for debugging
			auto scriptView = m_ActiveScene->GetRegistry().view<Nebula::ScriptComponent>();
			if (scriptView.size() > 0)
			{
				ConsoleWindow::AddLog(fmt::format("Found {} entities with scripts", scriptView.size()), Nebula::LogLevel::LOG_INFO);
			}
			else
			{
				ConsoleWindow::AddLog("No script components found in scene", Nebula::LogLevel::LOG_WARN);
			}
			
			// Store the scene path so we can reload it when stopping
			if (!m_CurrentScenePath.empty())
			{
				// TODO:
				// Save current editor state before entering play mode
				// This allows us to restore it when exiting play mode
			}
		}
		else
		{
			// Exiting runtime mode
			NB_CORE_INFO("Runtime stopped");
			ConsoleWindow::AddLog("Runtime stopped", Nebula::LogLevel::LOG_INFO);
			
			// Stop script runtime
			if (m_ActiveScene)
			{
				m_ActiveScene->OnRuntimeStop();
			}
			
			// Just stop audio - let Scene destructor handle cleanup
			if (m_ActiveScene && m_ActiveScene->GetAudioEngine())
			{
				m_ActiveScene->GetAudioEngine()->StopAll();
			}
			
			// Reload the scene to reset physics state
			if (!m_CurrentScenePath.empty())
			{
				LoadSceneFromPath(m_CurrentScenePath);
			}
		}
	}

	void EditorLayer::CheckAndShowProjectBrowser()
	{
		// Check if a project is already loaded
		if (!Nebula::Project::GetActive())
		{
			m_ShowProjectBrowser = true;
			m_ProjectLoaded = false;
		}
		else
		{
			m_ProjectLoaded = true;
		}
	}

	void EditorLayer::OpenProject(const std::filesystem::path& projectPath)
	{
		NB_CORE_INFO("Opening project: {0}", projectPath.string());

		// Load the project
		Nebula::Project* project = Nebula::Project::Load(projectPath);
		if (!project)
		{
			NB_CORE_ERROR("Failed to load project!");
			return;
		}

		// Build the project scripts
		NB_CORE_INFO("Building project scripts...");
		if (!Nebula::ScriptBuilder::BuildProjectScripts(projectPath))
		{
			NB_CORE_WARN("Failed to build project scripts. Creating new project file...");
			Nebula::ScriptBuilder::GenerateProjectFile(projectPath);
		}

		// Load the script assembly
		std::filesystem::path scriptAssembly = projectPath / "Assets" / "Scripts" / "bin" / "Debug" / "Scripts.dll";
		if (std::filesystem::exists(scriptAssembly))
		{
			Nebula::ScriptEngine::LoadProjectAssembly(scriptAssembly);
		}
		else
		{
			NB_CORE_WARN("Script assembly not found at: {0}", scriptAssembly.string());
		}

		// Set the content browser to the project's assets folder
		ContentBrowser::SetContentPath(Nebula::Project::GetAssetDirectory().string());

		// Setup script file watching
		m_ScriptsDirectory = projectPath / "Assets" / "Scripts";
		if (std::filesystem::exists(m_ScriptsDirectory))
		{
			m_LastScriptModifyTime = std::filesystem::last_write_time(m_ScriptsDirectory);
		}

		// Load the start scene if specified
		if (!project->GetConfig().StartScene.empty())
		{
			std::filesystem::path scenePath = Nebula::Project::GetAssetFileSystemPath(project->GetConfig().StartScene);
			if (std::filesystem::exists(scenePath))
			{
				LoadSceneFromPath(scenePath.string());
			}
		}

		m_ProjectLoaded = true;
		NB_CORE_INFO("Project loaded successfully!");
	}

	void EditorLayer::CreateNewProject(const std::string& name, const std::filesystem::path& path)
	{
		NB_CORE_INFO("Creating new project: {0} at {1}", name, path.string());

		// Create the project
		Nebula::Project* project = Nebula::Project::New(path, name);
		if (!project)
		{
			NB_CORE_ERROR("Failed to create project!");
			return;
		}

		// Generate the script project file
		Nebula::ScriptBuilder::GenerateProjectFile(path);

		// Create a default script file
		std::filesystem::path scriptPath = path / "Scripts" / "ExampleScript.cs";
		std::ofstream scriptFile(scriptPath);
		if (scriptFile.is_open())
		{
			scriptFile << R"(using Nebula;

public class ExampleScript : ScriptBehavior
{
    public override void OnCreate()
    {
        Log.LogInfo("ExampleScript Created!");
    }

    public override void OnUpdate(float deltaTime)
    {
        // Update logic here
    }
}
)";
			scriptFile.close();
		}

		// Set the content browser to the project's assets folder
		ContentBrowser::SetContentPath(Nebula::Project::GetAssetDirectory().string());

		// Create a default scene
		NewScene();
		m_CurrentScenePath = (Nebula::Project::GetAssetDirectory() / "Scenes" / "NewScene.nebscene").string();
		SaveScene();

		m_ProjectLoaded = true;
		NB_CORE_INFO("Project created successfully!");
	}

	void EditorLayer::CheckScriptFileChanges()
	{
		if (!std::filesystem::exists(m_ScriptsDirectory))
			return;

		try
		{
			auto currentTime = std::filesystem::last_write_time(m_ScriptsDirectory);
			
			// Also check all .cs files for changes
			bool filesChanged = false;
			for (const auto& entry : std::filesystem::recursive_directory_iterator(m_ScriptsDirectory))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".cs")
				{
					auto fileTime = std::filesystem::last_write_time(entry.path());
					if (fileTime > m_LastScriptModifyTime)
					{
						filesChanged = true;
						break;
					}
				}
			}

			if (filesChanged)
			{
				NB_CORE_INFO("Script file changes detected, rebuilding...");
				ConsoleWindow::AddLog("Script changes detected, rebuilding...", Nebula::LogLevel::LOG_INFO);
				m_LastScriptModifyTime = std::filesystem::file_time_type::clock::now();
				RebuildScripts();
			}
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			NB_CORE_ERROR("Error checking script files: {0}", e.what());
		}
	}

	void EditorLayer::RebuildScripts()
	{
		auto projectPath = Nebula::Project::GetProjectDirectory();
		bool success = Nebula::ScriptBuilder::RebuildScripts(projectPath);
		
		if (success)
		{
			m_HasBuildErrors = false;
			ConsoleWindow::AddLog("Scripts rebuilt successfully!", Nebula::LogLevel::LOG_INFO);
			NB_CORE_INFO("Scripts rebuilt successfully!");
		}
		else
		{
			m_HasBuildErrors = true;
			std::string errorMsg = Nebula::ScriptBuilder::GetLastBuildError();
			ConsoleWindow::AddLog("Scripts build failed!", Nebula::LogLevel::LOG_ERROR);
			if (!errorMsg.empty())
			{
				ConsoleWindow::AddLog(errorMsg, Nebula::LogLevel::LOG_ERROR);
			}
			NB_CORE_ERROR("Failed to rebuild scripts");
		}
	}

}

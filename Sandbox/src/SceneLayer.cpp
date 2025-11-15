#include "SceneLayer.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Sandbox {

	SceneLayer::SceneLayer()
		: Layer("SceneLayer"),
		  m_Scene(nullptr),
		  m_CameraPosition(0.0f, 0.0f, 5.0f),
		  m_CameraRotation(0.0f, 0.0f, 0.0f),
		  m_LastMouseX(0.0f),
		  m_LastMouseY(0.0f),
		  m_FirstMouse(true)
	{
	}

	SceneLayer::~SceneLayer()
	{
	}

	void SceneLayer::OnAttach()
	{
		// Create the scene
		m_Scene = new Nebula::Scene("Sandbox Scene");

		// Setup example scene content
		SetupExampleScene();

		// Set initial camera position
		auto& camera = static_cast<Nebula::PerspectiveCamera&>(Nebula::Application::Get().GetCamera());
		camera.SetPosition(m_CameraPosition);
		camera.SetRotation(m_CameraRotation);
	}

	void SceneLayer::OnDetach()
	{
		delete m_Scene;
	}

	void SceneLayer::SetupExampleScene()
	{
		// Create example entities
		auto cubeEntity = m_Scene->CreateEntity("Cube");
		
		// Add components to the cube
		auto& transform = cubeEntity.GetComponent<Nebula::TransformComponent>();
		transform.Position = glm::vec3(0.0f, 0.0f, 0.0f);
		transform.Scale = glm::vec3(1.0f);

		// You can add mesh renderer component here if you have meshes/materials setup
		// auto& meshRenderer = cubeEntity.AddComponent<Nebula::MeshRendererComponent>();
		// meshRenderer.Mesh = Nebula::Mesh::CreateCube();
		// meshRenderer.Material = yourMaterial;

		// Create more entities
		auto lightEntity = m_Scene->CreateEntity("Light");
		auto& lightTransform = lightEntity.GetComponent<Nebula::TransformComponent>();
		lightTransform.Position = glm::vec3(2.0f, 3.0f, 2.0f);
	}

	void SceneLayer::OnUpdate(Nebula::Timestep ts)
	{
		// Camera movement
		float moveSpeed = 2.5f * ts;

		// Mouse rotation (right-click drag)
		if (Nebula::Input::IsMouseButtonPressed(NB_MOUSE_BUTTON_2))
		{
			// Hide and lock cursor
			Nebula::Application::Get().GetWindow().SetCursorMode(true);

			auto [mouseX, mouseY] = Nebula::Input::GetMousePos();

			if (m_FirstMouse)
			{
				m_LastMouseX = mouseX;
				m_LastMouseY = mouseY;
				m_FirstMouse = false;
			}

			float xOffset = mouseX - m_LastMouseX;
			float yOffset = m_LastMouseY - mouseY;
			m_LastMouseX = mouseX;
			m_LastMouseY = mouseY;

			float sensitivity = 0.1f;
			xOffset *= sensitivity;
			yOffset *= sensitivity;

			m_CameraRotation.y -= xOffset;
			m_CameraRotation.x += yOffset;

			// Constrain pitch
			if (m_CameraRotation.x > 89.0f)
				m_CameraRotation.x = 89.0f;
			if (m_CameraRotation.x < -89.0f)
				m_CameraRotation.x = -89.0f;

			// Calculate camera direction vectors
			float pitch = glm::radians(m_CameraRotation.x);
			float yaw = glm::radians(m_CameraRotation.y);

			glm::mat4 rotation = glm::mat4(1.0f);
			rotation = glm::rotate(rotation, yaw, glm::vec3(0, 1, 0));
			rotation = glm::rotate(rotation, pitch, glm::vec3(1, 0, 0));

			glm::vec3 forward = -glm::vec3(rotation[2]);
			glm::vec3 right = glm::vec3(rotation[0]);

			// WASD movement
			if (Nebula::Input::IsKeyPressed(NB_KEY_W))
				m_CameraPosition += forward * moveSpeed;
			if (Nebula::Input::IsKeyPressed(NB_KEY_S))
				m_CameraPosition -= forward * moveSpeed;
			if (Nebula::Input::IsKeyPressed(NB_KEY_A))
				m_CameraPosition -= right * moveSpeed;
			if (Nebula::Input::IsKeyPressed(NB_KEY_D))
				m_CameraPosition += right * moveSpeed;

			// Update camera
			auto& camera = static_cast<Nebula::PerspectiveCamera&>(Nebula::Application::Get().GetCamera());
			camera.SetPosition(m_CameraPosition);
			camera.SetRotation(m_CameraRotation);
		}
		else
		{
			// Show cursor when not rotating
			Nebula::Application::Get().GetWindow().SetCursorMode(false);
			m_FirstMouse = true;
		}

		// Update the scene
		if (m_Scene)
		{
			m_Scene->OnUpdate(ts);
			auto& camera = Nebula::Application::Get().GetCamera();
			m_Scene->OnRender(camera);
		}
	}

	void SceneLayer::OnImGuiRender()
	{
		// Optional: Add ImGui debug info about the scene
	}

	void SceneLayer::OnEvent(Nebula::Event& e)
	{
	}

}



#include <Nebula.h>
#include <glm/gtc/matrix_transform.hpp>

class ExampleLayer : public Nebula::Layer {
public:
	ExampleLayer() 
		: Layer("Example"), 
		  m_Camera(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f),
		  m_CameraPosition(0.0f, 0.0f, 5.0f),
		  m_CameraRotation(0.0f, 0.0f, 0.0f),
		  m_LastMouseX(0.0f),
		  m_LastMouseY(0.0f),
		  m_FirstMouse(true)
	{
		// Textured cube
		m_SquareVA.reset(Nebula::VertexArray::Create());

		// Cube vertices: 24 vertices (4 per face * 6 faces)
		// Each vertex has: position (x, y, z), texcoord (u, v)
		float cubeVerts[5 * 24]{
			// Front face
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			
			// Back face
			 0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			
			// Left face
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			
			// Right face
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			
			// Top face
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			
			// Bottom face
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
		};

		std::shared_ptr<Nebula::VertexBuffer> squareVB;
		squareVB.reset(Nebula::VertexBuffer::Create(cubeVerts, sizeof(cubeVerts)));

		Nebula::BufferLayout squareLayout = {
			{Nebula::ShaderDataType::Float3, "a_Position"},
			{Nebula::ShaderDataType::Float2, "a_TexCoord"}
		};
		squareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(squareVB);

		// Cube indices: 6 faces * 2 triangles * 3 vertices = 36 indices
		unsigned int cubeIndices[36]{
			// Front
			0, 1, 2, 2, 3, 0,
			// Back
			4, 5, 6, 6, 7, 4,
			// Left
			8, 9, 10, 10, 11, 8,
			// Right
			12, 13, 14, 14, 15, 12,
			// Top
			16, 17, 18, 18, 19, 16,
			// Bottom
			20, 21, 22, 22, 23, 20
		};
		std::shared_ptr<Nebula::IndexBuffer> squareIB;
		squareIB.reset(Nebula::IndexBuffer::Create(cubeIndices, 36));
		m_SquareVA->SetIndexBuffer(squareIB);

		m_TextureShader.reset(Nebula::Shader::Create("assets/shaders/Texture.glsl"));
		m_Texture.reset(Nebula::Texture2D::Create("assets/textures/Checkerboard.png"));

		m_TextureShader->Bind();
		m_TextureShader->SetInt("u_Texture", 0);

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);
	}

	void OnUpdate() override {
		// Camera movement
		float moveSpeed = 2.5f * 0.016f; // Assuming ~60 FPS, adjust for delta time later

		// Mouse rotation (right-click drag)
		if (Nebula::Input::IsMouseButtonPressed(NB_MOUSE_BUTTON_2)) {
			// Hide and lock cursor
			Nebula::Application::Get().GetWindow().SetCursorMode(true);

			auto [mouseX, mouseY] = Nebula::Input::GetMousePos();

			if (m_FirstMouse) {
				m_LastMouseX = mouseX;
				m_LastMouseY = mouseY;
				m_FirstMouse = false;
			}

			float xOffset = mouseX - m_LastMouseX;
			float yOffset = m_LastMouseY - mouseY; // Reversed since y-coordinates range from bottom to top
			m_LastMouseX = mouseX;
			m_LastMouseY = mouseY;

			float sensitivity = 0.1f;
			xOffset *= sensitivity;
			yOffset *= sensitivity;

			m_CameraRotation.y -= xOffset; // Yaw (mouse right = look right)
			m_CameraRotation.x += yOffset; // Pitch (mouse up = look up)

			// Constrain pitch
			if (m_CameraRotation.x > 89.0f)
				m_CameraRotation.x = 89.0f;
			if (m_CameraRotation.x < -89.0f)
				m_CameraRotation.x = -89.0f;

			// Calculate camera direction vectors matching the camera's rotation order (yaw then pitch)
			float pitch = glm::radians(m_CameraRotation.x);
			float yaw = glm::radians(m_CameraRotation.y);

			// Build rotation matrix in same order as camera: yaw (Y) then pitch (X)
			glm::mat4 rotation = glm::mat4(1.0f);
			rotation = glm::rotate(rotation, yaw, glm::vec3(0, 1, 0));   // Yaw around Y
			rotation = glm::rotate(rotation, pitch, glm::vec3(1, 0, 0)); // Pitch around X

			// Extract forward direction from rotation matrix (negative Z axis in camera space)
			glm::vec3 forward = -glm::vec3(rotation[2]);
			
			// Extract right direction from rotation matrix (positive X axis in camera space)
			glm::vec3 right = glm::vec3(rotation[0]);

			// WASD movement relative to camera orientation
			if (Nebula::Input::IsKeyPressed(NB_KEY_W)) {
				m_CameraPosition += forward * moveSpeed;
			}
			if (Nebula::Input::IsKeyPressed(NB_KEY_S)) {
				m_CameraPosition -= forward * moveSpeed;
			}
			if (Nebula::Input::IsKeyPressed(NB_KEY_A)) {
				m_CameraPosition -= right * moveSpeed;
			}
			if (Nebula::Input::IsKeyPressed(NB_KEY_D)) {
				m_CameraPosition += right * moveSpeed;
			}

			// Vertical movement
			if (Nebula::Input::IsKeyPressed(NB_KEY_SPACE)) {
				m_CameraPosition.y += moveSpeed;
			}
			if (Nebula::Input::IsKeyPressed(NB_KEY_Q)) {
				m_CameraPosition.y -= moveSpeed;
			}
		}
		else {
			// Show cursor when not holding right-click
			Nebula::Application::Get().GetWindow().SetCursorMode(false);
			m_FirstMouse = true;
		}

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Nebula::Renderer::BeginScene(m_Camera);

		m_Texture->Bind();
		Nebula::Renderer::Submit(m_TextureShader, m_SquareVA);

		Nebula::Renderer::EndScene();
	}
	
	void OnEvent(Nebula::Event& event) override {
	}

private:
	std::shared_ptr<Nebula::Shader> m_TextureShader;
	std::shared_ptr<Nebula::Texture2D> m_Texture;
	std::shared_ptr<Nebula::VertexArray> m_SquareVA;

	Nebula::PerspectiveCamera m_Camera;
	glm::vec3 m_CameraPosition;
	glm::vec3 m_CameraRotation;

	float m_LastMouseX, m_LastMouseY;
	bool m_FirstMouse;
};

class Sandbox : public Nebula::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {

	}
};



Nebula::Application* Nebula::CreateApplication() {
	return new Sandbox();
}
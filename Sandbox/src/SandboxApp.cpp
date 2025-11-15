

#include <Nebula.h>

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
		// Textured square
		m_SquareVA.reset(Nebula::VertexArray::Create());

		float squareVerts[5 * 4]{
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		};

		std::shared_ptr<Nebula::VertexBuffer> squareVB;
		squareVB.reset(Nebula::VertexBuffer::Create(squareVerts, sizeof(squareVerts)));

		Nebula::BufferLayout squareLayout = {
			{Nebula::ShaderDataType::Float3, "a_Position"},
			{Nebula::ShaderDataType::Float2, "a_TexCoord"}
		};
		squareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(squareVB);

		unsigned int squareIndices[6]{ 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Nebula::IndexBuffer> squareIB;
		squareIB.reset(Nebula::IndexBuffer::Create(squareIndices, 6));
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

		// Calculate camera direction vectors using the same rotation order as the camera
		float pitch = glm::radians(m_CameraRotation.x);
		float yaw = glm::radians(m_CameraRotation.y);

		glm::vec3 forward;
		forward.x = cos(pitch) * sin(yaw);
		forward.y = sin(pitch);
		forward.z = cos(pitch) * cos(yaw);
		forward = glm::normalize(forward);

		glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), forward));
		glm::vec3 up = glm::normalize(glm::cross(forward, right));

		// WASD movement relative to camera orientation
		if (Nebula::Input::IsKeyPressed(NB_KEY_W)) {
			m_CameraPosition -= forward * moveSpeed;
		}
		if (Nebula::Input::IsKeyPressed(NB_KEY_S)) {
			m_CameraPosition += forward * moveSpeed;
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
		if (Nebula::Input::IsKeyPressed(NB_KEY_LEFT_SHIFT)) {
			m_CameraPosition.y -= moveSpeed;
		}

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

			m_CameraRotation.y -= xOffset; // Yaw (inverted: mouse right = look right)
			m_CameraRotation.x += yOffset; // Pitch (mouse up = look up)

			// Constrain pitch
			if (m_CameraRotation.x > 89.0f)
				m_CameraRotation.x = 89.0f;
			if (m_CameraRotation.x < -89.0f)
				m_CameraRotation.x = -89.0f;
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
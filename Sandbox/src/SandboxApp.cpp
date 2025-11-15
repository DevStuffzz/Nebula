

#include <Nebula.h>

class ExampleLayer : public Nebula::Layer {
public:
	ExampleLayer() : Layer("Example") {
		m_VertexArray.reset(Nebula::VertexArray::Create());

		float vertices[3 * 7]{
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.3f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		std::shared_ptr<Nebula::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Nebula::VertexBuffer::Create(vertices, sizeof(vertices)));

		Nebula::BufferLayout layout = {
			{Nebula::ShaderDataType::Float3, "a_Position"},
			{Nebula::ShaderDataType::Float4, "a_Color"}
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indices[3]{ 0, 1, 2 };
		std::shared_ptr<Nebula::IndexBuffer> indexBuffer;
		indexBuffer.reset(Nebula::IndexBuffer::Create(indices, 3));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		// Load shader from file
		m_Shader.reset(Nebula::Shader::Create("assets/shaders/Triangle.glsl"));

		// Square
		m_SquareVA.reset(Nebula::VertexArray::Create());

		float squareVerts[3 * 4]{
			-0.75f, -0.75f, 0.0f,
			 -0.25f, -0.75f, 0.0f,
			 -0.25f, -0.25f, 0.0f,
			-0.75f, -0.25f, 0.0f,
		};

		std::shared_ptr<Nebula::VertexBuffer> squareVB;
		squareVB.reset(Nebula::VertexBuffer::Create(squareVerts, sizeof(squareVerts)));

		Nebula::BufferLayout squareLayout = {
			{Nebula::ShaderDataType::Float3, "a_Position"}
		};
		squareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(squareVB);

		unsigned int squareIndices[6]{ 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Nebula::IndexBuffer> squareIB;
		squareIB.reset(Nebula::IndexBuffer::Create(squareIndices, 6));
		m_SquareVA->SetIndexBuffer(squareIB);

		m_SquareShader.reset(Nebula::Shader::Create("assets/shaders/FlatColor.glsl"));
	}

	void OnUpdate() override {
		Nebula::Renderer::BeginScene();

		Nebula::Renderer::Submit(m_SquareShader, m_SquareVA);
		Nebula::Renderer::Submit(m_Shader, m_VertexArray);

		Nebula::Renderer::EndScene();
	}
	
	void OnEvent(Nebula::Event& event) override {
	}

private:
	std::shared_ptr<Nebula::Shader> m_Shader;
	std::shared_ptr<Nebula::VertexArray> m_VertexArray;

	std::shared_ptr<Nebula::Shader> m_SquareShader;
	std::shared_ptr<Nebula::VertexArray> m_SquareVA;
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
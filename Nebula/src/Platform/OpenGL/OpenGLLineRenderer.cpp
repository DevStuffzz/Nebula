#include "nbpch.h"
#include "OpenGLLineRenderer.h"
#include "Nebula/Renderer/Shader.h"
#include "Nebula/Renderer/VertexArray.h"
#include "Nebula/Renderer/Buffer.h"
#include <glad/glad.h>

namespace Nebula {

	OpenGLLineRenderer::OpenGLLineRenderer()
		: m_ViewProjectionMatrix(glm::mat4(1.0f))
	{
		m_LineVertices.reserve(MaxVertices);

		// Create vertex array
		m_VertexArray.reset(VertexArray::Create());

		// Create vertex buffer with dummy data (will be updated each frame)
		std::vector<float> dummyData(MaxVertices * 6, 0.0f); // 6 floats per vertex (pos + color)
		m_VertexBuffer.reset(VertexBuffer::Create(dummyData.data(), (uint32_t)(dummyData.size() * sizeof(float))));
		
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Color" }
		};
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		// Create line shader
		const char* vertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec3 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Color;

			void main()
			{
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		const char* fragmentSrc = R"(
			#version 330 core
			layout(location = 0) out vec4 color;

			in vec3 v_Color;

			void main()
			{
				color = vec4(v_Color, 1.0);
			}
		)";

		m_LineShader.reset(Shader::Create(vertexSrc, fragmentSrc));
	}

	OpenGLLineRenderer::~OpenGLLineRenderer()
	{
	}

	void OpenGLLineRenderer::Begin()
	{
		m_LineVertices.clear();
	}

	void OpenGLLineRenderer::End()
	{
		Flush();
	}

	void OpenGLLineRenderer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color)
	{
		if (m_LineVertices.size() + 2 > MaxVertices)
		{
			Flush();
		}

		m_LineVertices.push_back({ start, color });
		m_LineVertices.push_back({ end, color });
	}

	void OpenGLLineRenderer::DrawLines(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& colors)
	{
		if (vertices.empty() || colors.empty())
			return;

		// Ensure we have enough space, flush if needed
		if (m_LineVertices.size() + vertices.size() > MaxVertices)
		{
			Flush();
		}

		// Add vertices with corresponding colors
		for (size_t i = 0; i < vertices.size(); ++i)
		{
			m_LineVertices.push_back({ vertices[i], colors[i % colors.size()] });

			// Flush if we're at capacity
			if (m_LineVertices.size() >= MaxVertices)
			{
				Flush();
			}
		}
	}

	void OpenGLLineRenderer::SetViewProjection(const glm::mat4& viewProjection)
	{
		m_ViewProjectionMatrix = viewProjection;
	}

	void OpenGLLineRenderer::Flush()
	{
		if (m_LineVertices.empty())
			return;

		// Update vertex buffer with new data
		m_VertexBuffer->Bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_LineVertices.size() * sizeof(LineVertex), m_LineVertices.data());

		// Bind shader and set uniforms
		m_LineShader->Bind();
		m_LineShader->SetMat4("u_ViewProjection", m_ViewProjectionMatrix);

		// Disable depth testing so lines render on top
		glDisable(GL_DEPTH_TEST);

		// Draw lines
		m_VertexArray->Bind();
		glDrawArrays(GL_LINES, 0, (GLsizei)m_LineVertices.size());

		// Re-enable depth testing
		glEnable(GL_DEPTH_TEST);

		m_LineVertices.clear();
	}

}

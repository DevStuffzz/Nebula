#include "nbpch.h"
#include "Mesh.h"
#include "Buffer.h"
#include <glm/gtc/constants.hpp>

namespace Nebula {

	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
		: m_Vertices(vertices), m_Indices(indices)
	{
		m_VertexArray.reset(VertexArray::Create());

		// Create vertex buffer
		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create((float*)m_Vertices.data(), (uint32_t)(m_Vertices.size() * sizeof(Vertex))));

		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_Normal" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		// Create index buffer
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size()));
		m_VertexArray->SetIndexBuffer(indexBuffer);
	}

	std::shared_ptr<Mesh> Mesh::CreateCube()
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		// Front face (z = 0.5)
		vertices.emplace_back(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		vertices.emplace_back(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		vertices.emplace_back(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		vertices.emplace_back(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		// Back face (z = -0.5)
		vertices.emplace_back(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		vertices.emplace_back(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		vertices.emplace_back(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f));

		// Left face (x = -0.5)
		vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		vertices.emplace_back(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		vertices.emplace_back(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		vertices.emplace_back(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

		// Right face (x = 0.5)
		vertices.emplace_back(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		vertices.emplace_back(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		vertices.emplace_back(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		vertices.emplace_back(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		// Top face (y = 0.5)
		vertices.emplace_back(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// Bottom face (y = -0.5)
		vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

		// Indices for 6 faces
		indices = {
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

		return std::make_shared<Mesh>(vertices, indices);
	}

	std::shared_ptr<Mesh> Mesh::CreateQuad()
	{
		std::vector<Vertex> vertices = {
			Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			Vertex(glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			Vertex(glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			Vertex(glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f))
		};

		std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0
		};

		return std::make_shared<Mesh>(vertices, indices);
	}

	std::shared_ptr<Mesh> Mesh::CreateSphere(uint32_t segments, uint32_t rings)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		const float pi = glm::pi<float>();
		const float twoPi = 2.0f * pi;

		// Generate vertices
		for (uint32_t ring = 0; ring <= rings; ++ring)
		{
			float theta = ring * pi / rings;
			float sinTheta = sin(theta);
			float cosTheta = cos(theta);

			for (uint32_t segment = 0; segment <= segments; ++segment)
			{
				float phi = segment * twoPi / segments;
				float sinPhi = sin(phi);
				float cosPhi = cos(phi);

				glm::vec3 normal(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
				glm::vec3 position = normal * 0.5f; // Radius of 0.5
				glm::vec2 texCoord((float)segment / segments, (float)ring / rings);

				vertices.emplace_back(position, texCoord, normal);
			}
		}

		// Generate indices
		for (uint32_t ring = 0; ring < rings; ++ring)
		{
			for (uint32_t segment = 0; segment < segments; ++segment)
			{
				uint32_t first = ring * (segments + 1) + segment;
				uint32_t second = first + segments + 1;

				indices.push_back(first);
				indices.push_back(second);
				indices.push_back(first + 1);

				indices.push_back(second);
				indices.push_back(second + 1);
				indices.push_back(first + 1);
			}
		}

		return std::make_shared<Mesh>(vertices, indices);
	}

}

#pragma once

#include "Nebula/Core.h"
#include "VertexArray.h"
#include <glm/glm.hpp>
#include <vector>

namespace Nebula {

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec3 Normal;

		Vertex(const glm::vec3& pos, const glm::vec2& texCoord = glm::vec2(0.0f), const glm::vec3& normal = glm::vec3(0.0f, 1.0f, 0.0f))
			: Position(pos), TexCoord(texCoord), Normal(normal) {}
	};

	class NEBULA_API Mesh
	{
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~Mesh() = default;

		void Bind() const { m_VertexArray->Bind(); }
		void Unbind() const { m_VertexArray->Unbind(); }

		const std::shared_ptr<VertexArray>& GetVertexArray() const { return m_VertexArray; }

		// Primitive mesh creation helpers
		static std::shared_ptr<Mesh> LoadOBJ(const std::string& path);
		static std::shared_ptr<Mesh> CreateCube();
		static std::shared_ptr<Mesh> CreateQuad();
		static std::shared_ptr<Mesh> CreateSphere();

	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};

}

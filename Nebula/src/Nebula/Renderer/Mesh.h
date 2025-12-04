#pragma once

#include "Nebula/Core.h"
#include "VertexArray.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace Nebula {

	using MeshID = uint32_t;

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

		// Mesh ID and source tracking
		MeshID GetID() const { return m_ID; }
		const std::string& GetSourcePath() const { return m_SourcePath; }
		void SetSourcePath(const std::string& path) { m_SourcePath = path; }

		// Primitive mesh creation helpers
		static std::shared_ptr<Mesh> LoadOBJ(const std::string& path);
		static std::shared_ptr<Mesh> CreateCube();
		static std::shared_ptr<Mesh> CreateQuad();
		static std::shared_ptr<Mesh> CreateSphere();

		// Mesh registry for serialization
		static std::shared_ptr<Mesh> GetByID(MeshID id);
		static MeshID GetOrCreateID(const std::string& sourcePath);
		static void RegisterMesh(MeshID id, std::shared_ptr<Mesh> mesh);

	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		MeshID m_ID = 0;
		std::string m_SourcePath;

		// Static registry
		static std::unordered_map<MeshID, std::shared_ptr<Mesh>> s_MeshRegistry;
		static std::unordered_map<std::string, MeshID> s_PathToID;
		static MeshID s_NextID;
	};

}

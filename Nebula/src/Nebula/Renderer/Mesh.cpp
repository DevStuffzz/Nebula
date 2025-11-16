#include "nbpch.h"
#include "Mesh.h"
#include "Buffer.h"
#include <glm/gtc/constants.hpp>

namespace Nebula {
	std::shared_ptr<Mesh> Mesh::LoadOBJ(const std::string& path)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;
		std::vector<uint32_t> vertexIndices;

		std::ifstream file(path);
		if (!file.is_open())
			return nullptr;

		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string prefix;
			iss >> prefix;
			if (prefix == "v") {
				float x, y, z;
				iss >> x >> y >> z;
				positions.emplace_back(x, y, z);
			} else if (prefix == "vt") {
				float u, v;
				iss >> u >> v;
				texCoords.emplace_back(u, v);
			} else if (prefix == "vn") {
				float nx, ny, nz;
				iss >> nx >> ny >> nz;
				normals.emplace_back(nx, ny, nz);
			} else if (prefix == "f") {
				std::vector<std::string> faceVerts;
				std::string vertStr;
				while (iss >> vertStr) {
					faceVerts.push_back(vertStr);
				}
				std::vector<uint32_t> faceIndices;
				for (const auto& vert : faceVerts) {
					std::istringstream viss(vert);
					std::string idxStr;
					std::vector<int> idxs;
					while (std::getline(viss, idxStr, '/')) {
						idxs.push_back(idxStr.empty() ? 0 : std::stoi(idxStr));
					}
					int posIdx = idxs.size() > 0 ? idxs[0] - 1 : -1;
					int texIdx = idxs.size() > 1 ? idxs[1] - 1 : -1;
					int normIdx = idxs.size() > 2 ? idxs[2] - 1 : -1;
					glm::vec3 pos = posIdx >= 0 ? positions[posIdx] : glm::vec3(0);
					glm::vec2 tex = texIdx >= 0 ? texCoords[texIdx] : glm::vec2(0);
					glm::vec3 norm = normIdx >= 0 ? normals[normIdx] : glm::vec3(0, 1, 0);
					vertices.emplace_back(pos, tex, norm);
					faceIndices.push_back((uint32_t)vertices.size() - 1);
				}
				// Triangulate face (fan method)
				for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
					indices.push_back(faceIndices[0]);
					indices.push_back(faceIndices[i]);
					indices.push_back(faceIndices[i + 1]);
				}
			}
		}
		file.close();
		return std::make_shared<Mesh>(vertices, indices);
	}

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
		return LoadOBJ("assets/models/Cube.obj");
	}

	std::shared_ptr<Mesh> Mesh::CreateQuad()
	{
		return LoadOBJ("assets/models/Quad.obj");
	}

	std::shared_ptr<Mesh> Mesh::CreateSphere()
	{
		return LoadOBJ("assets/models/Sphere.obj");
	}

}

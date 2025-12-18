#pragma once
#pragma warning(disable: 4251)
#pragma once 

#include "Nebula/Core.h"
#include "RenderCommand.h"

#include <glm/glm.hpp>

namespace Nebula {

	class Shader;
	class VertexArray;
	class Camera;
	class Material;
	class Mesh;

	class NEBULA_API Renderer {
	public:
		static void Init();

		static void BeginScene(Camera& camera);
		static void EndScene();
		
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const std::shared_ptr<Material>& material, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const std::shared_ptr<Material>& material, const std::shared_ptr<Mesh>& mesh, const glm::mat4& transform = glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* s_SceneData;
	};
}
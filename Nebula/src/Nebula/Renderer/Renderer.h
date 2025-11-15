#pragma once 

#include "Nebula/Core.h"
#include "RenderCommand.h"

namespace Nebula {

	class Shader;
	class VertexArray;
	class Camera;

	class NEBULA_API Renderer {
	public:
		static void Init();

		static void BeginScene(Camera& camera);
		static void EndScene();
		
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* s_SceneData;
	};
}
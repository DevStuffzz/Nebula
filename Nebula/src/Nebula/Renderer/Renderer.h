#pragma once 

#include "Nebula/Core.h"
#include "RenderCommand.h"

namespace Nebula {

	class Shader;
	class VertexArray;

	class NEBULA_API Renderer {
	public:
		static void Init();

		static void BeginScene();
		static void EndScene();
		
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};
}
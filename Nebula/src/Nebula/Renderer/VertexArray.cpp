#include "nbpch.h"


#include "VertexArray.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Nebula {
	VertexArray* VertexArray::Create() {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			NEB_CORE_ASSERT(false, "RendererAPI::None is not currently supported");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLVertexArray();
		}

		NEB_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}
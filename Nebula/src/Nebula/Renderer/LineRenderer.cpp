#include "nbpch.h"
#include "LineRenderer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLLineRenderer.h"

namespace Nebula {

	std::shared_ptr<LineRenderer> LineRenderer::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			NEB_CORE_ASSERT(false, "RendererAPI::None is not currently supported");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLLineRenderer>();
		}

		NEB_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}

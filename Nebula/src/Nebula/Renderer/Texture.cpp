#include "nbpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Nebula {

	Texture2D* Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    NEB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return new OpenGLTexture2D(path);
		}

		NEB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}

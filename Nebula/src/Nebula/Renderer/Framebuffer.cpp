#include "nbpch.h"
#include "Framebuffer.h"

#include "Nebula/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Nebula {

	Framebuffer* Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    NEB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return new OpenGLFramebuffer(spec);
		}

		NEB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}

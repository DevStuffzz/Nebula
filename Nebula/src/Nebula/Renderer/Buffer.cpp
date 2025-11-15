#include "nbpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Nebula/Core.h"


namespace Nebula {
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size) {
		switch (Renderer::GetAPI()) {
			case RendererAPI::None:		NEB_CORE_ASSERT(false, "RendererAPI::None is not currently supported");
			case RendererAPI::OpenGL:	return new OpenGLVertexBuffer(vertices, size);
			case RendererAPI::Direct3D:	NEB_CORE_ASSERT(false, "RendererAPI::Direct3D is not currently supported");
			case RendererAPI::Vulkan:	NEB_CORE_ASSERT(false, "RendererAPI::Vulkan is not currently supported");
			case RendererAPI::Metal:	NEB_CORE_ASSERT(false, "RendererAPI::Metal is not currently supported");
		}

		NEB_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI()) {
			case RendererAPI::None:		NEB_CORE_ASSERT(false, "RendererAPI::None is not currently supported");
			case RendererAPI::OpenGL:	return new OpenGLIndexBuffer(indices, count);
			case RendererAPI::Direct3D:	NEB_CORE_ASSERT(false, "RendererAPI::Direct3D is not currently supported");
			case RendererAPI::Vulkan:	NEB_CORE_ASSERT(false, "RendererAPI::Vulkan is not currently supported");
			case RendererAPI::Metal:	NEB_CORE_ASSERT(false, "RendererAPI::Metal is not currently supported");
		}

		NEB_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}
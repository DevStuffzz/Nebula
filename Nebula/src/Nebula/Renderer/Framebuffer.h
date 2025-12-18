#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Core.h"
#include <cstdint>

namespace Nebula {

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		uint32_t Samples = 1;
		bool SwapChainTarget = false;
	};

	class NEBULA_API Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

	virtual void Resize(uint32_t width, uint32_t height) = 0;

	virtual uint32_t GetColorAttachmentRendererID() const = 0;
	virtual uint32_t GetDepthAttachmentRendererID() const = 0;
	virtual const FramebufferSpecification& GetSpecification() const = 0;		static Framebuffer* Create(const FramebufferSpecification& spec);
	};

}

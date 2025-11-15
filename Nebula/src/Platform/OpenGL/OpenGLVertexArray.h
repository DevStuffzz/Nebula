#pragma once

#include "Nebula/Renderer/VertexArray.h"

namespace Nebula {
	class OpenGLVertexArray : public VertexArray {
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void AddVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer) override;
		virtual void SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer) override;


		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const override;
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override;

	private:
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	
	private:
		uint32_t m_RendererID;
	};
}
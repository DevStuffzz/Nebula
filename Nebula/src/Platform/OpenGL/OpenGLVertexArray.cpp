#include "nbpch.h"

#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Nebula {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::None:   return		 0;
		case ShaderDataType::Float:  return		 GL_FLOAT;
		case ShaderDataType::Float2: return		 GL_FLOAT;
		case ShaderDataType::Float3: return	     GL_FLOAT;
		case ShaderDataType::Float4: return		 GL_FLOAT;
		case ShaderDataType::Int:	 return		 GL_INT;
		case ShaderDataType::Int2:	 return		 GL_INT;
		case ShaderDataType::Int3:   return		 GL_INT;
		case ShaderDataType::Int4:   return		 GL_INT;
		case ShaderDataType::Mat3:   return		 GL_FLOAT;
		case ShaderDataType::Mat4:   return		 GL_FLOAT;
		case ShaderDataType::Bool:   return		 GL_BOOL;
		}

		NEB_CORE_ASSERT("Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind()
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind()
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer)
	{
		NEB_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no Layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();


		uint32_t index = 0;
		for (const auto& element : vertexBuffer->GetLayout()) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				vertexBuffer->GetLayout().GetStride(),
				(const void*)element.Offset
			);
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

	const std::vector<std::shared_ptr<VertexBuffer>>& OpenGLVertexArray::GetVertexBuffers() const
	{
		return m_VertexBuffers;
	}

	const std::shared_ptr<IndexBuffer>& OpenGLVertexArray::GetIndexBuffer() const
	{
		return m_IndexBuffer;
	}
}
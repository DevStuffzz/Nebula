#pragma once

#include "Nebula/Renderer/LineRenderer.h"
#include <glm/glm.hpp>

namespace Nebula {

	class Shader;
	class VertexArray;
	class VertexBuffer;

	class OpenGLLineRenderer : public LineRenderer {
	public:
		OpenGLLineRenderer();
		~OpenGLLineRenderer();

		virtual void Begin() override;
		virtual void End() override;

		virtual void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) override;
		virtual void DrawLines(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& colors) override;

		virtual void SetViewProjection(const glm::mat4& viewProjection) override;

	private:
		void Flush();

		struct LineVertex {
			glm::vec3 Position;
			glm::vec3 Color;
		};

		std::vector<LineVertex> m_LineVertices;
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<Shader> m_LineShader;
		glm::mat4 m_ViewProjectionMatrix;

		static const uint32_t MaxLines = 10000;
		static const uint32_t MaxVertices = MaxLines * 2;
	};

}

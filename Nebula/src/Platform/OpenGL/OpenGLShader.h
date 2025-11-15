#pragma once

#include "Nebula/Renderer/Shader.h"

namespace Nebula {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) override;

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<uint32_t, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<uint32_t, std::string>& shaderSources);

	private:
		uint32_t m_RendererID;
	};
}

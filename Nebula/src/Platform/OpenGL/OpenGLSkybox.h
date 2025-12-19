#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Renderer/Skybox.h"
#include <string>
#include <array>

namespace Nebula {

	class OpenGLSkybox : public Skybox
	{
	public:
		OpenGLSkybox(const std::string& directoryPath);
		virtual ~OpenGLSkybox();

		virtual void Bind() const override;
		virtual void Render() const override;
		virtual uint32_t GetRendererID() const override { return m_CubemapID; }

		const std::string& GetDirectoryPath() const { return m_DirectoryPath; }
		uint32_t GetShaderProgram() const { return m_ShaderProgram; }

	private:
		bool ValidateDirectory(const std::string& directoryPath);
		void LoadCubemap(const std::string& directoryPath);
		void CreateSkyboxGeometry();
		void CreateShaders();

	private:
		std::string m_DirectoryPath;
		uint32_t m_CubemapID = 0;
		uint32_t m_VAO = 0;
		uint32_t m_VBO = 0;
		uint32_t m_ShaderProgram = 0;
	};

}

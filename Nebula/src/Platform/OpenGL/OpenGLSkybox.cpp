#include "nbpch.h"
#include "OpenGLSkybox.h"
#include "Nebula/Log.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <filesystem>
#include <algorithm>

namespace Nebula {

	namespace {
		const char* vertexShaderSource = R"(
			#version 330 core
			layout (location = 0) in vec3 aPos;

			out vec3 TexCoords;

			uniform mat4 projection;
			uniform mat4 view;

			void main()
			{
				TexCoords = aPos;
				vec4 pos = projection * view * vec4(aPos, 1.0);
				gl_Position = pos.xyww; // Ensure skybox is always at far plane
			}
		)";

		const char* fragmentShaderSource = R"(
			#version 330 core
			out vec4 FragColor;

			in vec3 TexCoords;

			uniform samplerCube skybox;

			void main()
			{
				FragColor = texture(skybox, TexCoords);
			}
		)";

		// Skybox vertices (cube centered at origin)
		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
	}

	OpenGLSkybox::OpenGLSkybox(const std::string& directoryPath)
		: m_DirectoryPath(directoryPath)
	{
		if (!ValidateDirectory(directoryPath))
		{
			NB_CORE_ERROR("Skybox directory validation failed: {0}", directoryPath);
			return;
		}

		LoadCubemap(directoryPath);
		CreateSkyboxGeometry();
		CreateShaders();
	}

	OpenGLSkybox::~OpenGLSkybox()
	{
		if (m_CubemapID)
			glDeleteTextures(1, &m_CubemapID);
		if (m_VAO)
			glDeleteVertexArrays(1, &m_VAO);
		if (m_VBO)
			glDeleteBuffers(1, &m_VBO);
		if (m_ShaderProgram)
			glDeleteProgram(m_ShaderProgram);
	}

	bool OpenGLSkybox::ValidateDirectory(const std::string& directoryPath)
	{
		namespace fs = std::filesystem;

		if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath))
		{
			NB_CORE_ERROR("Skybox directory does not exist: {0}", directoryPath);
			return false;
		}

		// Required files (must have at least one extension)
		std::array<std::string, 6> requiredFiles = {
			"right", "left", "top", "bottom", "front", "back"
		};

		std::array<std::string, 2> validExtensions = { ".png", ".jpg" };

		// Check each required file exists with valid extension
		for (const auto& filename : requiredFiles)
		{
			bool found = false;
			for (const auto& ext : validExtensions)
			{
				fs::path filePath = fs::path(directoryPath) / (filename + ext);
				if (fs::exists(filePath))
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				NB_CORE_ERROR("Skybox missing required file: {0}.png or {0}.jpg", filename);
				return false;
			}
		}

		// Count total files in directory
		int fileCount = 0;
		for (const auto& entry : fs::directory_iterator(directoryPath))
		{
			if (entry.is_regular_file())
				fileCount++;
		}

		// Should have exactly 6 files
		if (fileCount != 6)
		{
			NB_CORE_ERROR("Skybox directory should contain exactly 6 image files. Found: {0}", fileCount);
			return false;
		}

		return true;
	}

	void OpenGLSkybox::LoadCubemap(const std::string& directoryPath)
	{
		namespace fs = std::filesystem;

		glGenTextures(1, &m_CubemapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapID);

		// Order: right, left, top, bottom, front, back
		std::array<std::string, 6> faceFiles = {
			"right", "left", "top", "bottom", "front", "back"
		};

		// Temporarily disable flipping for cubemaps (we'll restore after)
		stbi_set_flip_vertically_on_load(false);

		for (unsigned int i = 0; i < faceFiles.size(); i++)
		{
			// Try PNG first, then JPG
			std::string filepath;
			fs::path pngPath = fs::path(directoryPath) / (faceFiles[i] + ".png");
			fs::path jpgPath = fs::path(directoryPath) / (faceFiles[i] + ".jpg");

			if (fs::exists(pngPath))
				filepath = pngPath.string();
			else if (fs::exists(jpgPath))
				filepath = jpgPath.string();
			else
			{
				NB_CORE_ERROR("Could not find skybox face: {0}", faceFiles[i]);
				continue;
			}

			int width, height, channels;
			unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

			if (data)
			{
				GLenum format = GL_RGB;
				if (channels == 4)
					format = GL_RGBA;
				else if (channels == 3)
					format = GL_RGB;
				else if (channels == 1)
					format = GL_RED;

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

				NB_CORE_INFO("Loaded skybox face: {0} ({1}x{2}, {3} channels)", faceFiles[i], width, height, channels);
				stbi_image_free(data);
			}
			else
			{
				NB_CORE_ERROR("Failed to load skybox face: {0}", filepath);
				stbi_image_free(data);
			}
		}

		// Restore flipping for regular textures
		stbi_set_flip_vertically_on_load(true);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void OpenGLSkybox::CreateSkyboxGeometry()
	{
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glBindVertexArray(0);
	}

	void OpenGLSkybox::CreateShaders()
	{
		// Compile vertex shader
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			NB_CORE_ERROR("Skybox vertex shader compilation failed: {0}", infoLog);
		}

		// Compile fragment shader
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			NB_CORE_ERROR("Skybox fragment shader compilation failed: {0}", infoLog);
		}

		// Link shaders
		m_ShaderProgram = glCreateProgram();
		glAttachShader(m_ShaderProgram, vertexShader);
		glAttachShader(m_ShaderProgram, fragmentShader);
		glLinkProgram(m_ShaderProgram);

		glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(m_ShaderProgram, 512, NULL, infoLog);
			NB_CORE_ERROR("Skybox shader program linking failed: {0}", infoLog);
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void OpenGLSkybox::Bind() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapID);
	}

	void OpenGLSkybox::Render() const
	{
		if (m_CubemapID == 0 || m_VAO == 0 || m_ShaderProgram == 0)
			return;

		glDepthFunc(GL_LEQUAL);
		glUseProgram(m_ShaderProgram);

		glBindVertexArray(m_VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapID);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
	}

}

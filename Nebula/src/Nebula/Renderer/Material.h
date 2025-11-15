#pragma once

#include "Nebula/Core.h"
#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace Nebula {

	enum class MaterialPropertyType
	{
		None = 0,
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Mat3,
		Mat4,
		Texture2D
	};

	class NEBULA_API Material
	{
	public:
		Material(const std::shared_ptr<Shader>& shader);
		virtual ~Material() = default;

		void Bind();
		void Unbind();

		// Set material properties
		void SetFloat(const std::string& name, float value);
		void SetFloat2(const std::string& name, const glm::vec2& value);
		void SetFloat3(const std::string& name, const glm::vec3& value);
		void SetFloat4(const std::string& name, const glm::vec4& value);
		void SetInt(const std::string& name, int value);
		void SetMat3(const std::string& name, const glm::mat3& value);
		void SetMat4(const std::string& name, const glm::mat4& value);
		void SetTexture(const std::string& name, const std::shared_ptr<Texture2D>& texture);

		// Get material properties
		float GetFloat(const std::string& name) const;
		glm::vec2 GetFloat2(const std::string& name) const;
		glm::vec3 GetFloat3(const std::string& name) const;
		glm::vec4 GetFloat4(const std::string& name) const;
		int GetInt(const std::string& name) const;
		glm::mat3 GetMat3(const std::string& name) const;
		glm::mat4 GetMat4(const std::string& name) const;
		std::shared_ptr<Texture2D> GetTexture(const std::string& name) const;

		// Convenience methods for common properties
		void SetColor(const std::string& name, const glm::vec4& color) { SetFloat4(name, color); }
		void SetAlbedo(const glm::vec4& color) { SetFloat4("u_Color", color); }
		void SetAlbedoTexture(const std::shared_ptr<Texture2D>& texture) { SetTexture("u_Texture", texture); }
		void SetMetallic(float value) { SetFloat("u_Metallic", value); }
		void SetRoughness(float value) { SetFloat("u_Roughness", value); }
		void SetEmission(const glm::vec3& color) { SetFloat3("u_Emission", color); }

		std::shared_ptr<Shader> GetShader() { return m_Shader; }
		const std::shared_ptr<Shader> GetShader() const { return m_Shader; }

	private:
		void UploadUniforms();

		struct MaterialProperty
		{
			MaterialPropertyType Type;
			union
			{
				float FloatValue;
				glm::vec2 Float2Value;
				glm::vec3 Float3Value;
				glm::vec4 Float4Value;
				int IntValue;
				glm::mat3 Mat3Value;
				glm::mat4 Mat4Value;
			};
			std::shared_ptr<Texture2D> TextureValue;

			MaterialProperty() : Type(MaterialPropertyType::None), FloatValue(0.0f) {}
		};

		std::shared_ptr<Shader> m_Shader;
		std::unordered_map<std::string, MaterialProperty> m_Properties;
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_Textures;
	};

}

#include "nbpch.h"
#include "Material.h"
#include "Nebula/Log.h"

namespace Nebula {

	Material::Material(const std::shared_ptr<Shader>& shader)
		: m_Shader(shader)
	{
		NEB_CORE_ASSERT(shader, "Shader is null!");
	}

	void Material::Bind()
	{
		m_Shader->Bind();
		UploadUniforms();
	}

	void Material::Unbind()
	{
		m_Shader->Unbind();
	}

	void Material::SetFloat(const std::string& name, float value)
	{
		MaterialProperty& prop = m_Properties[name];
		prop.Type = MaterialPropertyType::Float;
		prop.FloatValue = value;
	}

	void Material::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		MaterialProperty& prop = m_Properties[name];
		prop.Type = MaterialPropertyType::Float2;
		prop.Float2Value = value;
	}

	void Material::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		MaterialProperty& prop = m_Properties[name];
		prop.Type = MaterialPropertyType::Float3;
		prop.Float3Value = value;
	}

	void Material::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		MaterialProperty& prop = m_Properties[name];
		prop.Type = MaterialPropertyType::Float4;
		prop.Float4Value = value;
	}

	void Material::SetInt(const std::string& name, int value)
	{
		MaterialProperty& prop = m_Properties[name];
		prop.Type = MaterialPropertyType::Int;
		prop.IntValue = value;
	}

	void Material::SetMat3(const std::string& name, const glm::mat3& value)
	{
		MaterialProperty& prop = m_Properties[name];
		prop.Type = MaterialPropertyType::Mat3;
		prop.Mat3Value = value;
	}

	void Material::SetMat4(const std::string& name, const glm::mat4& value)
	{
		MaterialProperty& prop = m_Properties[name];
		prop.Type = MaterialPropertyType::Mat4;
		prop.Mat4Value = value;
	}

	void Material::SetTexture(const std::string& name, const std::shared_ptr<Texture2D>& texture)
	{
		m_Textures[name] = texture;
	}

	float Material::GetFloat(const std::string& name) const
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end() && it->second.Type == MaterialPropertyType::Float)
			return it->second.FloatValue;
		return 0.0f;
	}

	glm::vec2 Material::GetFloat2(const std::string& name) const
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end() && it->second.Type == MaterialPropertyType::Float2)
			return it->second.Float2Value;
		return glm::vec2(0.0f);
	}

	glm::vec3 Material::GetFloat3(const std::string& name) const
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end() && it->second.Type == MaterialPropertyType::Float3)
			return it->second.Float3Value;
		return glm::vec3(0.0f);
	}

	glm::vec4 Material::GetFloat4(const std::string& name) const
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end() && it->second.Type == MaterialPropertyType::Float4)
			return it->second.Float4Value;
		return glm::vec4(0.0f);
	}

	int Material::GetInt(const std::string& name) const
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end() && it->second.Type == MaterialPropertyType::Int)
			return it->second.IntValue;
		return 0;
	}

	glm::mat3 Material::GetMat3(const std::string& name) const
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end() && it->second.Type == MaterialPropertyType::Mat3)
			return it->second.Mat3Value;
		return glm::mat3(1.0f);
	}

	glm::mat4 Material::GetMat4(const std::string& name) const
	{
		auto it = m_Properties.find(name);
		if (it != m_Properties.end() && it->second.Type == MaterialPropertyType::Mat4)
			return it->second.Mat4Value;
		return glm::mat4(1.0f);
	}

	std::shared_ptr<Texture2D> Material::GetTexture(const std::string& name) const
	{
		auto it = m_Textures.find(name);
		if (it != m_Textures.end())
			return it->second;
		return nullptr;
	}

	void Material::UploadUniforms()
	{
		// Upload all material properties to the shader
		for (const auto& [name, prop] : m_Properties)
		{
			switch (prop.Type)
			{
			case MaterialPropertyType::Float:
				m_Shader->SetFloat(name, prop.FloatValue);
				break;
			case MaterialPropertyType::Float2:
				m_Shader->SetFloat2(name, prop.Float2Value);
				break;
			case MaterialPropertyType::Float3:
				m_Shader->SetFloat3(name, prop.Float3Value);
				break;
			case MaterialPropertyType::Float4:
				m_Shader->SetFloat4(name, prop.Float4Value);
				break;
			case MaterialPropertyType::Int:
				m_Shader->SetInt(name, prop.IntValue);
				break;
			case MaterialPropertyType::Mat3:
				m_Shader->SetMat3(name, prop.Mat3Value);
				break;
			case MaterialPropertyType::Mat4:
				m_Shader->SetMat4(name, prop.Mat4Value);
				break;
			}
		}

		// Bind textures
		int textureSlot = 0;
		for (const auto& [name, texture] : m_Textures)
		{
			if (texture)
			{
				texture->Bind(textureSlot);
				m_Shader->SetInt(name, textureSlot);
				textureSlot++;
			}
		}
	}

}

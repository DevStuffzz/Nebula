#pragma once

#include "Nebula/Asset/AssetManager.h"
#include "Nebula/Renderer/Shader.h"

namespace Nebula {

	class NEBULA_API ShaderImporter : public AssetImporter
	{
	public:
		virtual std::shared_ptr<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata) override;
		virtual bool CanImport(const std::string& extension) const override;
	};

	// Shader wrapper that inherits from Asset
	class NEBULA_API ShaderAsset : public Asset
	{
	public:
		ShaderAsset(AssetHandle handle, const std::string& path, Shader* shader)
			: Asset(handle, AssetType::Shader, path), m_Shader(shader)
		{
			m_IsLoaded = (shader != nullptr);
		}

		~ShaderAsset()
		{
			delete m_Shader;
		}

		Shader* GetShader() const { return m_Shader; }
		
		void Bind() const 
		{ 
			if (m_Shader) 
				m_Shader->Bind(); 
		}

		void Unbind() const 
		{ 
			if (m_Shader) 
				m_Shader->Unbind(); 
		}

		static AssetType GetStaticType() { return AssetType::Shader; }

	private:
		Shader* m_Shader = nullptr;
	};

}

#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Asset/AssetManager.h"
#include "Nebula/Renderer/Texture.h"

namespace Nebula {

	class NEBULA_API TextureImporter : public AssetImporter
	{
	public:
		virtual std::shared_ptr<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata) override;
		virtual bool CanImport(const std::string& extension) const override;
	};

	// Texture2D wrapper that inherits from Asset
	class NEBULA_API TextureAsset : public Asset
	{
	public:
		TextureAsset(AssetHandle handle, const std::string& path, Texture2D* texture)
			: Asset(handle, AssetType::Texture2D, path), m_Texture(texture)
		{
			m_IsLoaded = (texture != nullptr);
		}

		~TextureAsset()
		{
			delete m_Texture;
		}

		Texture2D* GetTexture() const { return m_Texture; }
		
		uint32_t GetWidth() const { return m_Texture ? m_Texture->GetWidth() : 0; }
		uint32_t GetHeight() const { return m_Texture ? m_Texture->GetHeight() : 0; }
		
		void Bind(uint32_t slot = 0) const 
		{ 
			if (m_Texture) 
				m_Texture->Bind(slot); 
		}

		static AssetType GetStaticType() { return AssetType::Texture2D; }

	private:
		Texture2D* m_Texture = nullptr;
	};

}

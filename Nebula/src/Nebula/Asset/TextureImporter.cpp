#include "nbpch.h"
#include "TextureImporter.h"
#include "Nebula/Log.h"

namespace Nebula {

	std::shared_ptr<Asset> TextureImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		NB_CORE_INFO("Importing texture: {0}", metadata.FilePath);

		Texture2D* texture = Texture2D::Create(metadata.FilePath);
		
		if (!texture)
		{
			NB_CORE_ERROR("Failed to load texture: {0}", metadata.FilePath);
			return nullptr;
		}

		return std::make_shared<TextureAsset>(handle, metadata.FilePath, texture);
	}

	bool TextureImporter::CanImport(const std::string& extension) const
	{
		return extension == ".png" || 
		       extension == ".jpg" || 
		       extension == ".jpeg" || 
		       extension == ".tga" || 
		       extension == ".bmp";
	}

}

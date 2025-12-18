#pragma once
#pragma warning(disable: 4251)

#include "AssetManager.h"
#include "TextureImporter.h"
#include "ShaderImporter.h"

namespace Nebula {

	// Helper class to register all asset importers
	class AssetManagerRegistry
	{
	public:
		static void RegisterImporters()
		{
			AssetManager::RegisterImporter(AssetType::Texture2D, std::make_shared<TextureImporter>());
			AssetManager::RegisterImporter(AssetType::Shader, std::make_shared<ShaderImporter>());
			// Add more importers as they're implemented
		}
	};

}

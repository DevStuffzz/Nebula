#include "nbpch.h"
#include "AssetSerializer.h"
#include "Nebula/Log.h"
#include <filesystem>

namespace Nebula {

	bool AssetSerializer::SerializeMetadata(const AssetMetadata& metadata, const std::string& metaFilePath)
	{
		std::ofstream file(metaFilePath);
		
		if (!file.is_open())
		{
			NB_CORE_ERROR("Failed to create .meta file: {0}", metaFilePath);
			return false;
		}

		// Write simple key-value format
		file << "AssetHandle: " << metadata.Handle.Value << "\n";
		file << "AssetType: " << AssetTypeToString(metadata.Type) << "\n";
		file << "FilePath: " << metadata.FilePath << "\n";
		file << "IsMemoryAsset: " << (metadata.IsMemoryAsset ? "true" : "false") << "\n";

		file.close();
		return true;
	}

	AssetMetadata AssetSerializer::DeserializeMetadata(const std::string& metaFilePath)
	{
		AssetMetadata metadata;

		std::ifstream file(metaFilePath);
		if (!file.is_open())
		{
			NB_CORE_ERROR("Failed to open .meta file: {0}", metaFilePath);
			return metadata;
		}

		std::string line;
		while (std::getline(file, line))
		{
			// Parse key-value pairs
			size_t colonPos = line.find(':');
			if (colonPos == std::string::npos)
				continue;

			std::string key = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 2); // Skip ": "

			if (key == "AssetHandle")
			{
				metadata.Handle = AssetHandle(std::stoull(value));
			}
			else if (key == "AssetType")
			{
				metadata.Type = StringToAssetType(value);
			}
			else if (key == "FilePath")
			{
				metadata.FilePath = value;
			}
			else if (key == "IsMemoryAsset")
			{
				metadata.IsMemoryAsset = (value == "true");
			}
		}

		file.close();
		return metadata;
	}

	std::string AssetSerializer::AssetTypeToString(AssetType type)
	{
		return Nebula::AssetTypeToString(type);
	}

	AssetType AssetSerializer::StringToAssetType(const std::string& str)
	{
		if (str == "Texture2D") return AssetType::Texture2D;
		if (str == "Shader") return AssetType::Shader;
		if (str == "Mesh") return AssetType::Mesh;
		if (str == "Material") return AssetType::Material;
		if (str == "Scene") return AssetType::Scene;
		if (str == "Audio") return AssetType::Audio;
		if (str == "Prefab") return AssetType::Prefab;
		if (str == "Script") return AssetType::Script;
		
		return AssetType::None;
	}

}

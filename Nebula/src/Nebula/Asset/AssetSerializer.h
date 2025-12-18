#pragma once
#pragma warning(disable: 4251)

#include "Asset.h"
#include <string>
#include <fstream>
#include <sstream>

namespace Nebula {

	// Simple key-value serializer for .meta files
	class NEBULA_API AssetSerializer
	{
	public:
		static bool SerializeMetadata(const AssetMetadata& metadata, const std::string& metaFilePath);
		static AssetMetadata DeserializeMetadata(const std::string& metaFilePath);

	private:
		static std::string AssetTypeToString(AssetType type);
		static AssetType StringToAssetType(const std::string& str);
	};

}

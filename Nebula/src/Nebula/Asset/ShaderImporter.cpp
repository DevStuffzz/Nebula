#include "nbpch.h"
#include "ShaderImporter.h"
#include "Nebula/Log.h"

namespace Nebula {

	std::shared_ptr<Asset> ShaderImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		NB_CORE_INFO("Importing shader: {0}", metadata.FilePath);

		Shader* shader = Shader::Create(metadata.FilePath);
		
		if (!shader)
		{
			NB_CORE_ERROR("Failed to load shader: {0}", metadata.FilePath);
			return nullptr;
		}

		return std::make_shared<ShaderAsset>(handle, metadata.FilePath, shader);
	}

	bool ShaderImporter::CanImport(const std::string& extension) const
	{
		return extension == ".glsl" || extension == ".shader";
	}

}

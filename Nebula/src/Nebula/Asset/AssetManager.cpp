#include "nbpch.h"
#include "AssetManager.h"
#include "Nebula/Log.h"
#include <random>
#include <fstream>
#include <filesystem>

namespace Nebula {

	AssetManager* AssetManager::s_Instance = nullptr;

	void AssetManager::Init()
	{
		if (!s_Instance)
		{
			s_Instance = new AssetManager();
			NB_CORE_INFO("AssetManager initialized");
		}
	}

	void AssetManager::Shutdown()
	{
		if (s_Instance)
		{
			s_Instance->m_LoadedAssets.clear();
			s_Instance->m_AssetRegistry.clear();
			s_Instance->m_Importers.clear();
			
			delete s_Instance;
			s_Instance = nullptr;
			
			NB_CORE_INFO("AssetManager shutdown");
		}
	}

	std::shared_ptr<Asset> AssetManager::GetAssetByHandle(AssetHandle handle)
	{
		if (!s_Instance)
		{
			NB_CORE_ERROR("AssetManager not initialized!");
			return nullptr;
		}

		// Check if already loaded
		auto it = s_Instance->m_LoadedAssets.find(handle);
		if (it != s_Instance->m_LoadedAssets.end())
			return it->second;

		// Asset not loaded, try to load it
		auto metaIt = s_Instance->m_AssetRegistry.find(handle);
		if (metaIt == s_Instance->m_AssetRegistry.end())
		{
			NB_CORE_ERROR("Asset handle {0} not found in registry", handle.Value);
			return nullptr;
		}

		return LoadAssetFromPath(metaIt->second.FilePath);
	}

	std::shared_ptr<Asset> AssetManager::LoadAssetFromPath(const std::string& filepath)
	{
		if (!s_Instance)
		{
			NB_CORE_ERROR("AssetManager not initialized!");
			return nullptr;
		}

		// Check if already loaded by filepath
		AssetHandle existingHandle = GetAssetHandleFromPath(filepath);
		if (existingHandle.IsValid())
		{
			auto it = s_Instance->m_LoadedAssets.find(existingHandle);
			if (it != s_Instance->m_LoadedAssets.end())
				return it->second;
		}

		// Get asset type from extension
		std::filesystem::path path(filepath);
		std::string extension = path.extension().string();
		AssetType type = GetAssetTypeFromExtension(extension);

		if (type == AssetType::None)
		{
			NB_CORE_WARN("Unknown asset type for file: {0}", filepath);
			return nullptr;
		}

		// Find appropriate importer
		auto importerIt = s_Instance->m_Importers.find(type);
		if (importerIt == s_Instance->m_Importers.end())
		{
			NB_CORE_ERROR("No importer registered for asset type: {0}", AssetTypeToString(type));
			return nullptr;
		}

		// Generate or retrieve asset handle
		AssetHandle handle = existingHandle.IsValid() ? existingHandle : GenerateAssetHandle();

		// Create metadata
		AssetMetadata metadata(handle, type, filepath);
		
		// Import the asset
		std::shared_ptr<Asset> asset = importerIt->second->ImportAsset(handle, metadata);
		
		if (asset)
		{
			metadata.IsLoaded = true;
			s_Instance->m_AssetRegistry[handle] = metadata;
			s_Instance->m_LoadedAssets[handle] = asset;
			
			NB_CORE_INFO("Loaded asset: {0} (Handle: {1})", filepath, handle.Value);
		}
		else
		{
			NB_CORE_ERROR("Failed to import asset: {0}", filepath);
		}

		return asset;
	}

	AssetHandle AssetManager::ImportAsset(const std::string& filepath)
	{
		if (!s_Instance)
		{
			NB_CORE_ERROR("AssetManager not initialized!");
			return AssetHandle(0);
		}

		// Check if already registered
		AssetHandle existingHandle = GetAssetHandleFromPath(filepath);
		if (existingHandle.IsValid())
			return existingHandle;

		// Create new asset entry
		std::filesystem::path path(filepath);
		std::string extension = path.extension().string();
		AssetType type = GetAssetTypeFromExtension(extension);

		if (type == AssetType::None)
		{
			NB_CORE_WARN("Cannot import unknown asset type: {0}", filepath);
			return AssetHandle(0);
		}

		AssetHandle handle = GenerateAssetHandle();
		AssetMetadata metadata(handle, type, filepath);
		
		s_Instance->m_AssetRegistry[handle] = metadata;
		
		NB_CORE_INFO("Imported asset: {0} (Handle: {1})", filepath, handle.Value);
		
		return handle;
	}

	void AssetManager::RegisterAsset(const AssetMetadata& metadata, std::shared_ptr<Asset> asset)
	{
		if (!s_Instance)
		{
			NB_CORE_ERROR("AssetManager not initialized!");
			return;
		}

		s_Instance->m_AssetRegistry[metadata.Handle] = metadata;
		s_Instance->m_LoadedAssets[metadata.Handle] = asset;
	}

	void AssetManager::UnloadAsset(AssetHandle handle)
	{
		if (!s_Instance) return;

		auto it = s_Instance->m_LoadedAssets.find(handle);
		if (it != s_Instance->m_LoadedAssets.end())
		{
			s_Instance->m_LoadedAssets.erase(it);
			
			// Update metadata
			auto metaIt = s_Instance->m_AssetRegistry.find(handle);
			if (metaIt != s_Instance->m_AssetRegistry.end())
				metaIt->second.IsLoaded = false;
			
			NB_CORE_INFO("Unloaded asset with handle: {0}", handle.Value);
		}
	}

	AssetMetadata& AssetManager::GetMetadata(AssetHandle handle)
	{
		static AssetMetadata s_NullMetadata;
		
		if (!s_Instance)
			return s_NullMetadata;

		auto it = s_Instance->m_AssetRegistry.find(handle);
		if (it != s_Instance->m_AssetRegistry.end())
			return it->second;

		return s_NullMetadata;
	}

	AssetMetadata AssetManager::GetMetadataFromPath(const std::string& filepath)
	{
		if (!s_Instance)
			return AssetMetadata();

		for (auto& [handle, metadata] : s_Instance->m_AssetRegistry)
		{
			if (metadata.FilePath == filepath)
				return metadata;
		}

		return AssetMetadata();
	}

	AssetHandle AssetManager::GetAssetHandleFromPath(const std::string& filepath)
	{
		if (!s_Instance)
			return AssetHandle(0);

		for (auto& [handle, metadata] : s_Instance->m_AssetRegistry)
		{
			if (metadata.FilePath == filepath)
				return handle;
		}

		return AssetHandle(0);
	}

	bool AssetManager::IsAssetHandleValid(AssetHandle handle)
	{
		if (!s_Instance || !handle.IsValid())
			return false;

		return s_Instance->m_AssetRegistry.find(handle) != s_Instance->m_AssetRegistry.end();
	}

	bool AssetManager::IsAssetLoaded(AssetHandle handle)
	{
		if (!s_Instance)
			return false;

		return s_Instance->m_LoadedAssets.find(handle) != s_Instance->m_LoadedAssets.end();
	}

	void AssetManager::RegisterImporter(AssetType type, std::shared_ptr<AssetImporter> importer)
	{
		if (!s_Instance)
		{
			NB_CORE_ERROR("AssetManager not initialized!");
			return;
		}

		s_Instance->m_Importers[type] = importer;
		NB_CORE_INFO("Registered importer for asset type: {0}", AssetTypeToString(type));
	}

	void AssetManager::SaveAssetRegistry()
	{
		// TODO: Implement JSON serialization of asset registry
		NB_CORE_WARN("AssetManager::SaveAssetRegistry() not yet implemented");
	}

	void AssetManager::LoadAssetRegistry()
	{
		// TODO: Implement JSON deserialization of asset registry
		NB_CORE_WARN("AssetManager::LoadAssetRegistry() not yet implemented");
	}

	AssetType AssetManager::GetAssetTypeFromExtension(const std::string& extension)
	{
		if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".tga" || extension == ".bmp")
			return AssetType::Texture2D;
		else if (extension == ".glsl" || extension == ".shader")
			return AssetType::Shader;
		else if (extension == ".obj" || extension == ".fbx" || extension == ".gltf" || extension == ".glb")
			return AssetType::Mesh;
		else if (extension == ".mat")
			return AssetType::Material;
		else if (extension == ".scene" || extension == ".nebscene")
			return AssetType::Scene;
		else if (extension == ".wav" || extension == ".mp3" || extension == ".ogg")
			return AssetType::Audio;
		else if (extension == ".prefab")
			return AssetType::Prefab;
		else if (extension == ".lua" || extension == ".cs")
			return AssetType::Script;

		return AssetType::None;
	}

	AssetHandle AssetManager::GenerateAssetHandle()
	{
		static std::random_device rd;
		static std::mt19937_64 gen(rd());
		static std::uniform_int_distribution<uint64_t> dis;

		return AssetHandle(dis(gen));
	}

}

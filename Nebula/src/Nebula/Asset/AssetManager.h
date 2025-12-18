#pragma once
#pragma warning(disable: 4251)

#include "Asset.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>

namespace Nebula {

	// Asset importer interface
	class NEBULA_API AssetImporter
	{
	public:
		virtual ~AssetImporter() = default;
		virtual std::shared_ptr<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata) = 0;
		virtual bool CanImport(const std::string& extension) const = 0;
	};

	// Asset manager - centralized asset loading and management
	class NEBULA_API AssetManager
	{
	public:
		static void Init();
		static void Shutdown();

		// Asset loading
		template<typename T>
		static std::shared_ptr<T> GetAsset(AssetHandle handle);
		
		template<typename T>
		static std::shared_ptr<T> LoadAsset(const std::string& filepath);

		static std::shared_ptr<Asset> GetAssetByHandle(AssetHandle handle);
		static std::shared_ptr<Asset> LoadAssetFromPath(const std::string& filepath);

		// Asset creation (runtime/memory assets)
		template<typename T, typename... Args>
		static std::shared_ptr<T> CreateAsset(Args&&... args);

		// Asset registration
		static AssetHandle ImportAsset(const std::string& filepath);
		static void RegisterAsset(const AssetMetadata& metadata, std::shared_ptr<Asset> asset);
		static void UnloadAsset(AssetHandle handle);

		// Asset metadata
		static AssetMetadata& GetMetadata(AssetHandle handle);
		static AssetMetadata GetMetadataFromPath(const std::string& filepath);
		static AssetHandle GetAssetHandleFromPath(const std::string& filepath);
		static bool IsAssetHandleValid(AssetHandle handle);
		static bool IsAssetLoaded(AssetHandle handle);

		// Importer registration
		static void RegisterImporter(AssetType type, std::shared_ptr<AssetImporter> importer);

		// Asset database operations
		static void SaveAssetRegistry();
		static void LoadAssetRegistry();
		static std::unordered_map<AssetHandle, AssetMetadata>& GetAssetRegistry() { return s_Instance->m_AssetRegistry; }

		// Utility
		static AssetType GetAssetTypeFromExtension(const std::string& extension);
		static AssetHandle GenerateAssetHandle();

	private:
		AssetManager() = default;

		std::unordered_map<AssetHandle, AssetMetadata> m_AssetRegistry;
		std::unordered_map<AssetHandle, std::shared_ptr<Asset>> m_LoadedAssets;
		std::unordered_map<AssetType, std::shared_ptr<AssetImporter>> m_Importers;

		static AssetManager* s_Instance;
	};

	// Template implementations
	template<typename T>
	inline std::shared_ptr<T> AssetManager::GetAsset(AssetHandle handle)
	{
		auto asset = GetAssetByHandle(handle);
		return std::static_pointer_cast<T>(asset);
	}

	template<typename T>
	inline std::shared_ptr<T> AssetManager::LoadAsset(const std::string& filepath)
	{
		auto asset = LoadAssetFromPath(filepath);
		return std::static_pointer_cast<T>(asset);
	}

	template<typename T, typename... Args>
	inline std::shared_ptr<T> AssetManager::CreateAsset(Args&&... args)
	{
		AssetHandle handle = GenerateAssetHandle();
		auto asset = std::make_shared<T>(handle, std::forward<Args>(args)...);
		
		AssetMetadata metadata;
		metadata.Handle = handle;
		metadata.Type = T::GetStaticType();
		metadata.IsMemoryAsset = true;
		metadata.IsLoaded = true;

		s_Instance->m_AssetRegistry[handle] = metadata;
		s_Instance->m_LoadedAssets[handle] = asset;

		return asset;
	}

}

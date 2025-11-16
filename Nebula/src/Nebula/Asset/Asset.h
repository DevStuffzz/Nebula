#pragma once

#include "Nebula/Core.h"
#include <string>
#include <cstdint>

namespace Nebula {

	// Unique identifier for assets (128-bit UUID)
	struct NEBULA_API AssetHandle
	{
		uint64_t Value = 0;

		AssetHandle() = default;
		AssetHandle(uint64_t value) : Value(value) {}

		operator uint64_t() const { return Value; }
		bool operator==(const AssetHandle& other) const { return Value == other.Value; }
		bool operator!=(const AssetHandle& other) const { return Value != other.Value; }
		bool IsValid() const { return Value != 0; }
	};

	enum class AssetType : uint16_t
	{
		None = 0,
		Texture2D,
		Shader,
		Mesh,
		Material,
		Scene,
		Audio,
		Prefab,
		Script
	};

	// Asset metadata stored in .meta files
	struct NEBULA_API AssetMetadata
	{
		AssetHandle Handle;
		AssetType Type = AssetType::None;
		std::string FilePath;
		bool IsLoaded = false;
		bool IsMemoryAsset = false; // Assets created at runtime (not from file)

		AssetMetadata() = default;
		AssetMetadata(AssetHandle handle, AssetType type, const std::string& path)
			: Handle(handle), Type(type), FilePath(path) {}
	};

	// Base class for all engine assets
	class NEBULA_API Asset
	{
	public:
		virtual ~Asset() = default;

		AssetHandle GetHandle() const { return m_Handle; }
		AssetType GetType() const { return m_Type; }
		const std::string& GetPath() const { return m_FilePath; }
		
		bool IsValid() const { return m_Handle.IsValid(); }
		bool IsLoaded() const { return m_IsLoaded; }

		static AssetType GetStaticType() { return AssetType::None; }

	protected:
		Asset(AssetHandle handle, AssetType type, const std::string& path = "")
			: m_Handle(handle), m_Type(type), m_FilePath(path), m_IsLoaded(false) {}

		AssetHandle m_Handle = 0;
		AssetType m_Type = AssetType::None;
		std::string m_FilePath;
		bool m_IsLoaded = false;

		friend class AssetManager;
	};

	// Helper to get asset type name as string
	inline const char* AssetTypeToString(AssetType type)
	{
		switch (type)
		{
			case AssetType::None:      return "None";
			case AssetType::Texture2D: return "Texture2D";
			case AssetType::Shader:    return "Shader";
			case AssetType::Mesh:      return "Mesh";
			case AssetType::Material:  return "Material";
			case AssetType::Scene:     return "Scene";
			case AssetType::Audio:     return "Audio";
			case AssetType::Prefab:    return "Prefab";
			case AssetType::Script:    return "Script";
		}
		return "Unknown";
	}

	// Helper to get file extension for asset type
	inline const char* AssetTypeToExtension(AssetType type)
	{
		switch (type)
		{
			case AssetType::Texture2D: return ".png";
			case AssetType::Shader:    return ".glsl";
			case AssetType::Mesh:      return ".obj";
			case AssetType::Material:  return ".mat";
			case AssetType::Scene:     return ".nebscene";
			case AssetType::Audio:     return ".wav";
			case AssetType::Prefab:    return ".prefab";
			case AssetType::Script:    return ".lua";
			default:                   return "";
		}
	}

}

// Hash function for AssetHandle (for use in unordered_map)
namespace std {
	template<>
	struct hash<Nebula::AssetHandle>
	{
		size_t operator()(const Nebula::AssetHandle& handle) const
		{
			return hash<uint64_t>()(handle.Value);
		}
	};
}

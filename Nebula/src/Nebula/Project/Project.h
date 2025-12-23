#pragma once

#include "Nebula/Core.h"
#include <string>
#include <filesystem>

namespace Nebula {

	struct ProjectConfig
	{
		std::string Name = "Untitled";
		std::string AssetDirectory = "Assets";
		std::string ScriptDirectory = "Scripts";
		std::string StartScene = "";
	};

	class NEBULA_API Project
	{
	public:
		static const std::filesystem::path& GetProjectDirectory()
		{
			return s_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
		}

		static std::filesystem::path GetScriptDirectory()
		{
			return GetProjectDirectory() / s_ActiveProject->m_Config.ScriptDirectory;
		}

		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
		{
			return GetAssetDirectory() / path;
		}

		const std::string& GetName() const { return m_Config.Name; }
		const ProjectConfig& GetConfig() const { return m_Config; }

		static Project* GetActive() { return s_ActiveProject; }
		static void SetActive(Project* project) { s_ActiveProject = project; }

		static Project* New(const std::filesystem::path& path, const std::string& name);
		static Project* Load(const std::filesystem::path& path);
		static bool SaveActive();

	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;

		inline static Project* s_ActiveProject = nullptr;
	};

}

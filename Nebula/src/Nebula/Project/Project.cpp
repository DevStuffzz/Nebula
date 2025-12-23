#include "nbpch.h"
#include "Project.h"
#include "Nebula/Log.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Nebula {

	Project* Project::New(const std::filesystem::path& path, const std::string& name)
	{
		Project* project = new Project();
		project->m_Config.Name = name;
		project->m_ProjectDirectory = path;

		// Create project directories
		std::filesystem::create_directories(GetAssetDirectory());
		std::filesystem::create_directories(GetAssetDirectory() / "Scenes");
		std::filesystem::create_directories(GetAssetDirectory() / "Textures");
		std::filesystem::create_directories(GetAssetDirectory() / "Models");
		std::filesystem::create_directories(GetAssetDirectory() / "Audio");
		std::filesystem::create_directories(GetScriptDirectory());

		// Create a default scene
		std::string defaultScenePath = "Scenes/NewScene.nebscene";
		project->m_Config.StartScene = defaultScenePath;

		s_ActiveProject = project;
		SaveActive();

		NB_CORE_INFO("Created new project: {0}", name);
		return project;
	}

	Project* Project::Load(const std::filesystem::path& path)
	{
		Project* project = new Project();

		std::filesystem::path projectFile = path / (path.filename().string() + ".nebproject");
		
		if (!std::filesystem::exists(projectFile))
		{
			NB_CORE_ERROR("Project file not found: {0}", projectFile.string());
			delete project;
			return nullptr;
		}

		std::ifstream file(projectFile);
		if (!file.is_open())
		{
			NB_CORE_ERROR("Failed to open project file: {0}", projectFile.string());
			delete project;
			return nullptr;
		}

		json j;
		file >> j;
		file.close();

		project->m_Config.Name = j.value("name", "Untitled");
		project->m_Config.AssetDirectory = j.value("assetDirectory", "Assets");
		project->m_Config.ScriptDirectory = j.value("scriptDirectory", "Scripts");
		project->m_Config.StartScene = j.value("startScene", "");
		project->m_ProjectDirectory = path;

		s_ActiveProject = project;

		NB_CORE_INFO("Loaded project: {0}", project->m_Config.Name);
		return project;
	}

	bool Project::SaveActive()
	{
		if (!s_ActiveProject)
			return false;

		std::filesystem::path projectFile = s_ActiveProject->m_ProjectDirectory / 
			(s_ActiveProject->m_Config.Name + ".nebproject");

		json j;
		j["name"] = s_ActiveProject->m_Config.Name;
		j["assetDirectory"] = s_ActiveProject->m_Config.AssetDirectory;
		j["scriptDirectory"] = s_ActiveProject->m_Config.ScriptDirectory;
		j["startScene"] = s_ActiveProject->m_Config.StartScene;

		std::ofstream file(projectFile);
		if (!file.is_open())
		{
			NB_CORE_ERROR("Failed to save project file: {0}", projectFile.string());
			return false;
		}

		file << j.dump(4);
		file.close();

		NB_CORE_INFO("Saved project: {0}", s_ActiveProject->m_Config.Name);
		return true;
	}

}

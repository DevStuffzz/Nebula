#include "nbpch.h"
#include "SceneManager.h"
#include "SceneSerializer.h"
#include "Nebula/Log.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace Nebula {

	SceneManager& SceneManager::Get()
	{
		static SceneManager instance;
		return instance;
	}

	std::shared_ptr<Scene> SceneManager::LoadScene(size_t index)
	{
		if (index >= m_SceneList.size())
		{
			NB_ERROR("SceneManager::LoadScene - Invalid scene index: {0}", index);
			return nullptr;
		}

		return LoadSceneFromPath(m_SceneList[index]);
	}

	std::shared_ptr<Scene> SceneManager::LoadSceneFromPath(const std::string& filepath)
	{
		auto scene = std::make_shared<Scene>();
		SceneSerializer serializer(scene.get());
		
		if (serializer.Deserialize(filepath))
		{
			m_ActiveScene = scene;
			NB_INFO("SceneManager: Loaded scene from {0}", filepath);
			return scene;
		}
		
		NB_ERROR("SceneManager: Failed to load scene from {0}", filepath);
		return nullptr;
	}

	void SceneManager::AddSceneToList(const std::string& scenePath)
	{
		// Check if scene already exists in list
		for (const auto& path : m_SceneList)
		{
			if (path == scenePath)
			{
				NB_WARN("SceneManager: Scene already in list: {0}", scenePath);
				return;
			}
		}

		m_SceneList.push_back(scenePath);
		NB_INFO("SceneManager: Added scene to list: {0}", scenePath);
	}

	void SceneManager::RemoveSceneFromList(size_t index)
	{
		if (index >= m_SceneList.size())
		{
			NB_ERROR("SceneManager: Invalid scene index for removal: {0}", index);
			return;
		}

		std::string removedScene = m_SceneList[index];
		m_SceneList.erase(m_SceneList.begin() + index);
		NB_INFO("SceneManager: Removed scene from list: {0}", removedScene);
	}

	void SceneManager::MoveSceneInList(size_t fromIndex, size_t toIndex)
	{
		if (fromIndex >= m_SceneList.size() || toIndex >= m_SceneList.size())
		{
			NB_ERROR("SceneManager: Invalid scene indices for move operation");
			return;
		}

		if (fromIndex == toIndex)
			return;

		std::string scene = m_SceneList[fromIndex];
		m_SceneList.erase(m_SceneList.begin() + fromIndex);
		m_SceneList.insert(m_SceneList.begin() + toIndex, scene);
	}

	void SceneManager::ClearSceneList()
	{
		m_SceneList.clear();
		NB_INFO("SceneManager: Cleared scene list");
	}

	bool SceneManager::SaveSceneList(const std::string& filepath)
	{
		nlohmann::json j;
		j["scenes"] = m_SceneList;

		std::ofstream file(filepath);
		if (!file.is_open())
		{
			NB_ERROR("SceneManager: Failed to open file for writing: {0}", filepath);
			return false;
		}

		file << j.dump(4); // Pretty print with 4 space indentation
		file.close();

		NB_INFO("SceneManager: Saved scene list to {0}", filepath);
		return true;
	}

	bool SceneManager::LoadSceneList(const std::string& filepath)
	{
		std::ifstream file(filepath);
		if (!file.is_open())
		{
			NB_WARN("SceneManager: Scene list file not found: {0}", filepath);
			return false;
		}

		try
		{
			nlohmann::json j;
			file >> j;
			file.close();

			m_SceneList.clear();
			if (j.contains("scenes") && j["scenes"].is_array())
			{
				for (const auto& scenePath : j["scenes"])
				{
					if (scenePath.is_string())
					{
						m_SceneList.push_back(scenePath.get<std::string>());
					}
				}
			}

			NB_INFO("SceneManager: Loaded scene list from {0} ({1} scenes)", filepath, m_SceneList.size());
			return true;
		}
		catch (const std::exception& e)
		{
			NB_ERROR("SceneManager: Failed to parse scene list: {0}", e.what());
			return false;
		}
	}

}

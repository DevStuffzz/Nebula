#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Core.h"
#include "Scene.h"
#include <string>
#include <vector>
#include <memory>

namespace Nebula {

	class NEBULA_API SceneManager
	{
	public:
		static SceneManager& Get();

		// Load scene by index from scene list
		std::shared_ptr<Scene> LoadScene(size_t index);
		
		// Load scene by path (not in scene list)
		std::shared_ptr<Scene> LoadSceneFromPath(const std::string& filepath);

		// Get the current active scene
		std::shared_ptr<Scene> GetActiveScene() { return m_ActiveScene; }

		// Scene list management
		void AddSceneToList(const std::string& scenePath);
		void RemoveSceneFromList(size_t index);
		void MoveSceneInList(size_t fromIndex, size_t toIndex);
		void ClearSceneList();
		
		const std::vector<std::string>& GetSceneList() const { return m_SceneList; }
		size_t GetSceneCount() const { return m_SceneList.size(); }
		
		// Serialize/Deserialize scene list
		bool SaveSceneList(const std::string& filepath = "assets/SceneList.json");
		bool LoadSceneList(const std::string& filepath = "assets/SceneList.json");

		// Deferred scene loading (to avoid destroying scene mid-update)
		void RequestLoadScene(size_t index);
		void ProcessPendingSceneLoad();
		bool HasPendingSceneLoad() const { return m_PendingSceneIndex >= 0; }

	private:
		SceneManager() = default;
		~SceneManager() = default;
		SceneManager(const SceneManager&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;

	private:
		std::shared_ptr<Scene> m_ActiveScene;
		std::vector<std::string> m_SceneList;
		int m_PendingSceneIndex = -1;
	};

}

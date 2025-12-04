#include "SceneListWindow.h"
#include <filesystem>

#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Scene/SceneManager.h"


namespace Cosmic {

	void SceneListWindow::OnImGuiRender()
	{
		if (!m_IsOpen)
			return;

		Nebula::NebulaGui::Begin("Scene List", &m_IsOpen);
		
		auto& sceneManager = Nebula::SceneManager::Get();
		const auto& sceneList = sceneManager.GetSceneList();

		Nebula::NebulaGui::Text("Scenes in Build:");
		Nebula::NebulaGui::Separator();

			// Display scene list
			for (size_t i = 0; i < sceneList.size(); i++)
			{
				std::string id = "scene_" + std::to_string(i);
				Nebula::NebulaGui::PushID(id.c_str());
				
				// Scene index
				Nebula::NebulaGui::Text("%d", (int)i);
				Nebula::NebulaGui::SameLine();

				// Scene path (clickable to highlight)
				if (Nebula::NebulaGui::Selectable(sceneList[i].c_str(), false))
				{
					// Could load the scene here if needed
				}
				Nebula::NebulaGui::SameLine();

				// Move up button
				if (i > 0)
				{
					if (Nebula::NebulaGui::Button("^"))
					{
						MoveSceneUp(i);
					}
					Nebula::NebulaGui::SameLine();
				}

				// Move down button
				if (i < sceneList.size() - 1)
				{
					if (Nebula::NebulaGui::Button("v"))
					{
						MoveSceneDown(i);
					}
					Nebula::NebulaGui::SameLine();
				}

				// Remove button
				if (Nebula::NebulaGui::Button("Remove"))
				{
					RemoveScene(i);
				}

				Nebula::NebulaGui::PopID();
			}

			Nebula::NebulaGui::Separator();

			// Add scene button
			if (Nebula::NebulaGui::Button("Add Scene"))
			{
				AddSceneDialog();
			}

			Nebula::NebulaGui::SameLine();

			// Save scene list button
			if (Nebula::NebulaGui::Button("Save Scene List"))
			{
				sceneManager.SaveSceneList();
			}

			Nebula::NebulaGui::SameLine();

			// Load scene list button
			if (Nebula::NebulaGui::Button("Load Scene List"))
			{
				sceneManager.LoadSceneList();
			}

			Nebula::NebulaGui::SameLine();

			// Clear scene list button
			if (Nebula::NebulaGui::Button("Clear"))
			{
				sceneManager.ClearSceneList();
		}

		Nebula::NebulaGui::Text("Total Scenes: %d", (int)sceneList.size());
		
		Nebula::NebulaGui::End();
	}

	void SceneListWindow::AddSceneDialog()
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "Nebula Scene\0*.nebscene\0All Files\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = "assets\\scenes";
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			std::string filepath = ofn.lpstrFile;
			
			// Convert to relative path if possible
			std::filesystem::path absPath = std::filesystem::absolute(filepath);
			std::filesystem::path currentPath = std::filesystem::current_path();
			std::string relativePath = std::filesystem::relative(absPath, currentPath).string();
			
			// Replace backslashes with forward slashes for consistency
			std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
			
			Nebula::SceneManager::Get().AddSceneToList(relativePath);
		}
	}

	void SceneListWindow::RemoveScene(size_t index)
	{
		Nebula::SceneManager::Get().RemoveSceneFromList(index);
	}

	void SceneListWindow::MoveSceneUp(size_t index)
	{
		if (index > 0)
		{
			Nebula::SceneManager::Get().MoveSceneInList(index, index - 1);
		}
	}

	void SceneListWindow::MoveSceneDown(size_t index)
	{
		auto& sceneManager = Nebula::SceneManager::Get();
		if (index < sceneManager.GetSceneCount() - 1)
		{
			sceneManager.MoveSceneInList(index, index + 1);
		}
	}

}

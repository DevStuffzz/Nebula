#include "ProjectBrowser.h"
#include <imgui.h>
#include <Nebula/Log.h>
#include <Nebula/Core/FileDialog.h>
#include <fstream>
#include <chrono>
#include <ctime>

// Use simple JSON parsing instead
#include <sstream>
#include <string>
#include <algorithm>

namespace Cosmic {

	ProjectBrowser::ProjectBrowser()
	{
		LoadRecentProjects();
		
		// Set default project path to Documents/NebulaProjects
		char* userProfile = nullptr;
		size_t len = 0;
		_dupenv_s(&userProfile, &len, "USERPROFILE");
		if (userProfile)
		{
			std::filesystem::path documentsPath = std::filesystem::path(userProfile) / "Documents" / "NebulaProjects";
			strcpy_s(m_NewProjectPathBuffer, documentsPath.string().c_str());
			free(userProfile);
		}
	}

	bool ProjectBrowser::Show(bool* pOpen)
	{
		if (!*pOpen)
			return false;

		bool projectSelected = false;

		ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
		ImGui::Begin("Nebula Project Browser", pOpen, ImGuiWindowFlags_NoCollapse);

		// Tabs for different sections
		if (ImGui::BeginTabBar("ProjectTabs"))
		{
			if (ImGui::BeginTabItem("Recent Projects"))
			{
				RenderRecentProjects();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("New Project"))
			{
				RenderNewProjectPanel();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		// Check if project was selected
		if (!m_SelectedProjectPath.empty())
		{
			projectSelected = true;
			AddRecentProject(m_SelectedProjectPath);
			*pOpen = false;
		}

		if (m_CreateNewProject)
		{
			projectSelected = true;
			*pOpen = false;
		}

		ImGui::End();
		return projectSelected;
	}

	void ProjectBrowser::RenderRecentProjects()
	{
		ImGui::Text("Recent Projects:");
		ImGui::Separator();

		if (m_RecentProjects.empty())
		{
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No recent projects");
		}
		else
		{
			ImGui::BeginChild("RecentProjectsList", ImVec2(0, -40), true);

			for (size_t i = 0; i < m_RecentProjects.size(); i++)
			{
				const auto& project = m_RecentProjects[i];
				
				ImGui::PushID((int)i);
				
				// Project card
				ImGui::BeginGroup();
				
				ImGui::Text("%s", project.Name.c_str());
				ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s", project.Path.string().c_str());
				ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Last opened: %s", project.LastOpened.c_str());
				
				ImGui::SameLine(ImGui::GetWindowWidth() - 80);
				if (ImGui::Button("Open", ImVec2(70, 0)))
				{
					if (std::filesystem::exists(project.Path))
					{
						m_SelectedProjectPath = project.Path.string();
					}
					else
					{
						NB_CORE_ERROR("Project path does not exist: {0}", project.Path.string());
					}
				}
				
				ImGui::EndGroup();
				ImGui::Separator();
				
				ImGui::PopID();
			}

			ImGui::EndChild();
		}

		// Open existing project button
		if (ImGui::Button("Open Other Project...", ImVec2(-1, 30)))
		{
			// Note: We would need a folder picker. For now use file dialog to pick .nebproject file
			Nebula::FileDialog* dialog = Nebula::FileDialog::Create();
			auto result = dialog->OpenFile("Nebula Project\0*.nebproject\0All Files\0*.*\0", "");
			delete dialog;
			
			if (result.has_value())
			{
				// Get the directory containing the .nebproject file
				m_SelectedProjectPath = std::filesystem::path(result.value()).parent_path().string();
			}
		}
	}

	void ProjectBrowser::RenderNewProjectPanel()
	{
		ImGui::Text("Create a new Nebula project:");
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Project Name:");
		ImGui::SetNextItemWidth(-1);
		ImGui::InputText("##ProjectName", m_NewProjectNameBuffer, sizeof(m_NewProjectNameBuffer));

		ImGui::Spacing();
		ImGui::Text("Project Location:");
		ImGui::SetNextItemWidth(-80);
		ImGui::InputText("##ProjectPath", m_NewProjectPathBuffer, sizeof(m_NewProjectPathBuffer));
		ImGui::SameLine();
		if (ImGui::Button("Browse..."))
		{
			// Note: Would need folder picker. For now just let user type the path
			// TODO: Implement folder picker dialog
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// Full path preview
		std::filesystem::path fullPath = std::filesystem::path(m_NewProjectPathBuffer) / m_NewProjectNameBuffer;
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Project will be created at:");
		ImGui::TextWrapped("%s", fullPath.string().c_str());

		ImGui::Spacing();
		ImGui::Spacing();

		// Create button
		if (ImGui::Button("Create Project", ImVec2(-1, 40)))
		{
			m_NewProjectName = m_NewProjectNameBuffer;
			std::filesystem::path fullPath = std::filesystem::path(m_NewProjectPathBuffer) / m_NewProjectName;
			m_NewProjectPath = fullPath.string();
			m_CreateNewProject = true;

			AddRecentProject(fullPath);
		}
	}

	void ProjectBrowser::LoadRecentProjects()
	{
		char* appData = nullptr;
		size_t len = 0;
		_dupenv_s(&appData, &len, "APPDATA");
		if (!appData)
			return;
			
		std::filesystem::path configPath = std::filesystem::path(appData) / "Nebula" / "RecentProjects.txt";
		free(appData);
		
		if (!std::filesystem::exists(configPath))
			return;

		std::ifstream file(configPath);
		if (!file.is_open())
			return;

		std::string line;
		while (std::getline(file, line))
		{
			if (line.empty())
				continue;

			// Format: name|path|lastOpened
			size_t pos1 = line.find('|');
			if (pos1 == std::string::npos)
				continue;

			size_t pos2 = line.find('|', pos1 + 1);
			if (pos2 == std::string::npos)
				continue;

			RecentProject recent;
			recent.Name = line.substr(0, pos1);
			recent.Path = line.substr(pos1 + 1, pos2 - pos1 - 1);
			recent.LastOpened = line.substr(pos2 + 1);
			
			// Only add if path still exists
			if (std::filesystem::exists(recent.Path))
			{
				m_RecentProjects.push_back(recent);
			}
		}
		file.close();
	}

	void ProjectBrowser::SaveRecentProjects()
	{
		char* appData = nullptr;
		size_t len = 0;
		_dupenv_s(&appData, &len, "APPDATA");
		if (!appData)
			return;
			
		std::filesystem::path configDir = std::filesystem::path(appData) / "Nebula";
		free(appData);
		
		std::filesystem::create_directories(configDir);
		
		std::filesystem::path configPath = configDir / "RecentProjects.txt";

		std::ofstream file(configPath);
		if (file.is_open())
		{
			for (const auto& project : m_RecentProjects)
			{
				file << project.Name << "|" << project.Path.string() << "|" << project.LastOpened << "\n";
			}
			file.close();
		}
	}

	void ProjectBrowser::AddRecentProject(const std::filesystem::path& path)
	{
		// Get project name from path
		std::string name = path.filename().string();
		
		// Get current time
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		char timeStr[100];
		struct tm timeInfo;
		localtime_s(&timeInfo, &time);
		std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M", &timeInfo);

		// Remove if already exists
		m_RecentProjects.erase(
			std::remove_if(m_RecentProjects.begin(), m_RecentProjects.end(),
				[&path](const RecentProject& p) { return p.Path == path; }),
			m_RecentProjects.end()
		);

		// Add to front
		RecentProject recent;
		recent.Name = name;
		recent.Path = path;
		recent.LastOpened = timeStr;
		m_RecentProjects.insert(m_RecentProjects.begin(), recent);

		// Keep only 10 most recent
		if (m_RecentProjects.size() > 10)
			m_RecentProjects.resize(10);

		SaveRecentProjects();
	}

}

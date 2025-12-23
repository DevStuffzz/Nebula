#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace Cosmic {

	struct RecentProject
	{
		std::string Name;
		std::filesystem::path Path;
		std::string LastOpened;
	};

	class ProjectBrowser
	{
	public:
		ProjectBrowser();
		~ProjectBrowser() = default;

		// Returns true if a project was selected
		bool Show(bool* pOpen);

		const std::string& GetSelectedProjectPath() const { return m_SelectedProjectPath; }
		bool ShouldCreateNewProject() const { return m_CreateNewProject; }
		const std::string& GetNewProjectName() const { return m_NewProjectName; }
		const std::string& GetNewProjectPath() const { return m_NewProjectPath; }

	private:
		void LoadRecentProjects();
		void SaveRecentProjects();
		void AddRecentProject(const std::filesystem::path& path);

		void RenderRecentProjects();
		void RenderNewProjectPanel();

	private:
		std::vector<RecentProject> m_RecentProjects;
		std::string m_SelectedProjectPath;
		bool m_CreateNewProject = false;

		// New project data
		char m_NewProjectNameBuffer[256] = "NewProject";
		char m_NewProjectPathBuffer[512] = "";
		std::string m_NewProjectName;
		std::string m_NewProjectPath;

		bool m_ShowNewProjectPanel = false;
	};

}

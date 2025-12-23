#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <ctime>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

namespace fs = std::filesystem;

struct RecentProject
{
	std::string Name;
	std::string Path;
	std::time_t LastOpened;
};

class ProjectBrowser
{
public:
	ProjectBrowser()
	{
		// Get default project location
		char* userProfile = nullptr;
		size_t len = 0;
		_dupenv_s(&userProfile, &len, "USERPROFILE");
		if (userProfile)
		{
			m_DefaultPath = fs::path(userProfile) / "Documents" / "NebulaProjects";
			free(userProfile);
		}
		else
		{
			m_DefaultPath = "C:/NebulaProjects";
		}

		strncpy_s(m_NewProjectPath, m_DefaultPath.string().c_str(), sizeof(m_NewProjectPath) - 1);
		LoadRecentProjects();
	}

	void Render(bool* pOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Nebula Project Browser", pOpen, ImGuiWindowFlags_NoCollapse))
		{
			ImGui::End();
			return;
		}

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

		ImGui::End();
	}

	bool ShouldLaunchProject() const { return m_LaunchProject; }
	const std::string& GetProjectPath() const { return m_SelectedProjectPath; }

private:
	void RenderRecentProjects()
	{
		ImGui::Text("Recent Projects:");
		ImGui::Separator();

		if (m_RecentProjects.empty())
		{
			ImGui::TextDisabled("No recent projects");
		}
		else
		{
			for (size_t i = 0; i < m_RecentProjects.size(); i++)
			{
				const auto& proj = m_RecentProjects[i];
				
				ImGui::PushID((int)i);
				
				// Project name as button
				if (ImGui::Button(proj.Name.c_str(), ImVec2(200, 0)))
				{
					LaunchProject(proj.Path);
				}
				
				ImGui::SameLine();
				ImGui::TextDisabled("%s", proj.Path.c_str());
				
				ImGui::PopID();
			}
		}

		ImGui::Spacing();
		ImGui::Separator();
		
		if (ImGui::Button("Open Other Project..."))
		{
			std::string path = OpenFileDialog();
			if (!path.empty())
			{
				// OpenFileDialog returns the .nebproject file, we need the parent directory
				fs::path projectDir = fs::path(path).parent_path();
				LaunchProject(projectDir.string());
			}
		}
	}

	void RenderNewProjectPanel()
	{
		ImGui::Text("Create New Project");
		ImGui::Separator();

		ImGui::InputText("Project Name", m_NewProjectName, sizeof(m_NewProjectName));
		
		ImGui::Text("Project Location:");
		ImGui::InputText("##Path", m_NewProjectPath, sizeof(m_NewProjectPath));
		ImGui::SameLine();
		if (ImGui::Button("Browse..."))
		{
			std::string path = OpenFileDialog();
			if (!path.empty())
			{
				// Get the project's parent directory for the location field
				std::string parentPath = fs::path(path).parent_path().parent_path().string();
				strncpy_s(m_NewProjectPath, parentPath.c_str(), sizeof(m_NewProjectPath) - 1);
			}
		}

		ImGui::Spacing();
		ImGui::Text("Full Path: %s\\%s", m_NewProjectPath, m_NewProjectName);

		ImGui::Spacing();
		if (ImGui::Button("Create Project", ImVec2(120, 0)))
		{
			if (strlen(m_NewProjectName) > 0)
			{
				CreateProject();
			}
		}
	}

	void CreateProject()
	{
		fs::path projectPath = fs::path(m_NewProjectPath) / m_NewProjectName;
		fs::create_directories(projectPath);

		// Create project file
		fs::path projectFile = projectPath / (std::string(m_NewProjectName) + ".nebproject");
		
		nlohmann::json projectConfig;
		projectConfig["name"] = m_NewProjectName;
		projectConfig["assetDirectory"] = "Assets";
		projectConfig["scriptDirectory"] = "Assets/Scripts";
		projectConfig["startScene"] = "";

		std::ofstream file(projectFile);
		if (file.is_open())
		{
			file << projectConfig.dump(4);
			file.close();
		}

		// Create Assets folder structure (empty, for user content)
		fs::create_directories(projectPath / "Assets" / "Scenes");
		fs::create_directories(projectPath / "Assets" / "Textures");
		fs::create_directories(projectPath / "Assets" / "Models");
		fs::create_directories(projectPath / "Assets" / "Scripts");
		fs::create_directories(projectPath / "Assets" / "Audio");

		// Copy Library folder from launcher directory (contains engine/editor assets)
		char launcherPath[MAX_PATH];
		GetModuleFileNameA(nullptr, launcherPath, MAX_PATH);
		fs::path launcherDir = fs::path(launcherPath).parent_path();
		fs::path sourceLibraryPath = launcherDir / "Library";
		fs::path destLibraryPath = projectPath / "Library";

		if (fs::exists(sourceLibraryPath))
		{
			fs::create_directories(destLibraryPath);
			fs::copy(sourceLibraryPath, destLibraryPath, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
		}

		// Copy imgui.ini if it exists
		fs::path sourceImguiIni = launcherDir / "Cosmic" / "imgui.ini";
		fs::path destImguiIni = projectPath / "imgui.ini";
		if (fs::exists(sourceImguiIni))
		{
			fs::copy(sourceImguiIni, destImguiIni, fs::copy_options::overwrite_existing);
		}

		LaunchProject(projectPath.string());
	}

	void LaunchProject(const std::string& projectPath)
	{
		m_SelectedProjectPath = projectPath;
		m_LaunchProject = true;
		
		// Add to recent projects
		AddRecentProject(projectPath);
		SaveRecentProjects();
	}

	void LoadRecentProjects()
	{
		char* appData = nullptr;
		size_t len = 0;
		_dupenv_s(&appData, &len, "APPDATA");
		if (!appData) return;

		fs::path configPath = fs::path(appData) / "Nebula" / "RecentProjects.txt";
		free(appData);

		if (!fs::exists(configPath)) return;

		std::ifstream file(configPath);
		if (!file.is_open()) return;

		std::string line;
		while (std::getline(file, line))
		{
			if (line.empty()) continue;

			size_t pos1 = line.find('|');
			size_t pos2 = line.rfind('|');

			if (pos1 != std::string::npos && pos2 != std::string::npos && pos1 != pos2)
			{
				RecentProject proj;
				proj.Name = line.substr(0, pos1);
				proj.Path = line.substr(pos1 + 1, pos2 - pos1 - 1);
				proj.LastOpened = std::stoll(line.substr(pos2 + 1));

				if (fs::exists(proj.Path))
				{
					m_RecentProjects.push_back(proj);
				}
			}
		}

		file.close();
	}

	void SaveRecentProjects()
	{
		char* appData = nullptr;
		size_t len = 0;
		_dupenv_s(&appData, &len, "APPDATA");
		if (!appData) return;

		fs::path configDir = fs::path(appData) / "Nebula";
		free(appData);

		fs::create_directories(configDir);

		fs::path configPath = configDir / "RecentProjects.txt";
		std::ofstream file(configPath);
		if (!file.is_open()) return;

		for (const auto& proj : m_RecentProjects)
		{
			file << proj.Name << "|" << proj.Path << "|" << proj.LastOpened << "\n";
		}

		file.close();
	}

	void AddRecentProject(const std::string& projectPath)
	{
		fs::path path(projectPath);
		std::string name = path.filename().string();

		// Remove if already exists
		m_RecentProjects.erase(
			std::remove_if(m_RecentProjects.begin(), m_RecentProjects.end(),
				[&projectPath](const RecentProject& p) { return p.Path == projectPath; }),
			m_RecentProjects.end()
		);

		// Add to front
		RecentProject proj;
		proj.Name = name;
		proj.Path = projectPath;
		proj.LastOpened = std::time(nullptr);

		m_RecentProjects.insert(m_RecentProjects.begin(), proj);

		// Limit to 10
		if (m_RecentProjects.size() > 10)
		{
			m_RecentProjects.resize(10);
		}
	}

	std::string OpenFileDialog()
	{
#ifdef _WIN32
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "Nebula Project\0*.nebproject\0All Files\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
#endif
		return std::string();
	}

private:
	std::vector<RecentProject> m_RecentProjects;
	fs::path m_DefaultPath;
	char m_NewProjectPath[512];
	char m_NewProjectName[256] = "MyGame";
	
	bool m_LaunchProject = false;
	std::string m_SelectedProjectPath;
};

int main(int argc, char** argv)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return -1;
	}

	// Create window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Nebula Launcher", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	ProjectBrowser browser;
	bool showBrowser = true;

	// Main loop
	while (!glfwWindowShouldClose(window) && showBrowser)
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		browser.Render(&showBrowser);

		// Check if we should launch
		if (browser.ShouldLaunchProject())
		{
			std::string projectPath = browser.GetProjectPath();
			
			// Get launcher directory and find Cosmic.exe
			char launcherPath[MAX_PATH];
			GetModuleFileNameA(nullptr, launcherPath, MAX_PATH);
			fs::path launcherDir = fs::path(launcherPath).parent_path();
			fs::path cosmicPath = launcherDir / "Cosmic" / "Cosmic.exe";
		fs::path projectDir = fs::path(projectPath);

#ifdef _WIN32
		// Launch Cosmic with project path and working directory set to project folder
		std::string command = "\"" + cosmicPath.string() + "\" \"" + projectPath + "\"";
		STARTUPINFOA si = { sizeof(si) };
		PROCESS_INFORMATION pi;

		if (CreateProcessA(nullptr, const_cast<char*>(command.c_str()), nullptr, nullptr, FALSE, 0, nullptr, projectDir.string().c_str(), &si, &pi))
			{
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
#endif
			break;
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

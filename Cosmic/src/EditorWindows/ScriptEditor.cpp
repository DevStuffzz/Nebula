#include "nbpch.h"
#include "ScriptEditor.h"

#include "Nebula/ImGui/NebulaGui.h"
#include <Nebula/Log.h>
// #include <Nebula/Scripting/LuaScriptEngine.h>  // TODO: Re-enable when C# scripting is fixed
#include <Nebula/Input.h>
#include <Nebula/Keycodes.h>
#include <imgui.h>

#include <fstream>
#include <sstream>

using namespace Nebula;

namespace Cosmic {

	std::vector<ScriptTab> ScriptEditor::s_Tabs;
	int ScriptEditor::s_ActiveTab = -1;
	bool ScriptEditor::s_RuntimeMode = false;

	void ScriptEditor::NewScript()
	{
		ScriptTab tab;
		tab.Path = "";
		tab.Content = "-- New Lua Script\n-- Available APIs: Transform, Input, Log, Key, Mouse\n\nfunction OnCreate(entity)\n    Log.Info(\"Script created!\")\nend\n\nfunction OnUpdate(entity, deltaTime)\n    -- Update logic here\n    -- Example: Move with WASD\n    -- if Input.IsKeyPressed(Key.W) then\n    --     local pos = Transform.GetPosition(entity)\n    --     Transform.SetPosition(entity, pos.x, pos.y + 5 * deltaTime, pos.z)\n    -- end\nend\n\nfunction OnDestroy(entity)\n    -- Cleanup logic here\nend\n";
		tab.IsDirty = true;
		s_Tabs.push_back(tab);
		s_ActiveTab = s_Tabs.size() - 1;
	}

	void ScriptEditor::CloseTab(int tabIndex)
	{
		if (tabIndex < 0 || tabIndex >= s_Tabs.size())
			return;

		s_Tabs.erase(s_Tabs.begin() + tabIndex);
		if (s_ActiveTab >= s_Tabs.size())
			s_ActiveTab = s_Tabs.size() - 1;
	}

	void ScriptEditor::OnImGuiRender()
	{
		NebulaGui::Begin("Script Editor");

		// Show runtime lock message
		if (s_RuntimeMode)
		{
			NebulaGui::TextColored({1.0f, 0.5f, 0.0f, 1.0f}, "Script editing is locked during runtime");
			NebulaGui::Separator();
		}

		// File menu
		if (NebulaGui::BeginMenuBar())
		{
			if (NebulaGui::BeginMenu("File"))
			{
				if (NebulaGui::MenuItem("New Script", "Ctrl+N"))
				{
					NewScript();
				}
				if (NebulaGui::MenuItem("Open Script"))
				{
					// TODO: File dialog
				}
				if (s_ActiveTab >= 0 && NebulaGui::MenuItem("Save Script", "Ctrl+S"))
				{
					SaveScript();
				}
				if (s_ActiveTab >= 0 && NebulaGui::MenuItem("Close Tab", "Ctrl+W"))
				{
					CloseTab(s_ActiveTab);
				}
				if (s_ActiveTab >= 0 && NebulaGui::MenuItem("Compile Script", "F5"))
				{
					ReloadScript();
				}
				NebulaGui::EndMenu();
			}
			NebulaGui::EndMenuBar();
		}

		// Keyboard shortcuts
		if (NebulaGui::IsWindowFocused())
		{
			if (Input::IsKeyDown(NB_KEY_LEFT_CONTROL) && Input::IsKeyPressed(NB_KEY_S))
				SaveScript();
			if (Input::IsKeyDown(NB_KEY_LEFT_CONTROL) && Input::IsKeyPressed(NB_KEY_W))
				CloseTab(s_ActiveTab);
			if (Input::IsKeyDown(NB_KEY_LEFT_CONTROL) && Input::IsKeyPressed(NB_KEY_N))
				NewScript();
			if (Input::IsKeyPressed(NB_KEY_F5))
				ReloadScript();
		}

		if (NebulaGui::BeginTabBar("ScriptTabs"))
		{
			for (int i = (int)s_Tabs.size() - 1; i >= 0; i--)
			{
				std::string label = s_Tabs[i].Path.empty() ? "Untitled" : s_Tabs[i].Path.substr(s_Tabs[i].Path.find_last_of("/\\") + 1);
				if (s_Tabs[i].IsDirty)
					label += "*";

				std::string tabLabel = label + "##" + std::to_string(i);

				bool open = true;
				if (NebulaGui::BeginTabItem(tabLabel.c_str(), &open))
				{
					s_ActiveTab = i;

					int flags = 0;
					if (s_RuntimeMode)
						flags |= ImGuiInputTextFlags_ReadOnly;
					else
						flags |= ImGuiInputTextFlags_AllowTabInput;

					if (NebulaGui::InputTextMultiline("##ScriptContent", &s_Tabs[i].Content[0], s_Tabs[i].Content.capacity(), glm::vec2(-1, -1), flags))
					{
						if (!s_RuntimeMode)
							s_Tabs[i].IsDirty = true;
					}

					NebulaGui::EndTabItem();
				}

				if (!open)
					CloseTab(i);
			}
			NebulaGui::EndTabBar();
		}



		NebulaGui::End();
	}

	void ScriptEditor::LoadScript(const std::string& path)
	{
		// Check if already open
		for (size_t i = 0; i < s_Tabs.size(); i++)
		{
			if (s_Tabs[i].Path == path)
			{
				s_ActiveTab = (int)i;
				return;
			}
		}

		std::ifstream file(path);
		if (file.is_open())
		{
			ScriptTab tab;
			std::stringstream buffer;
			buffer << file.rdbuf();
			tab.Content = buffer.str();
			tab.Path = path;
			tab.IsDirty = false;
			file.close();

			s_Tabs.push_back(tab);
			s_ActiveTab = (int)(s_Tabs.size() - 1);
		}
		else
		{
			NB_CORE_ERROR("Failed to load script: {0}", path);
		}
	}

	void ScriptEditor::SaveScript(int tabIndex)
	{
		if (tabIndex < 0)
			tabIndex = s_ActiveTab;

		if (tabIndex < 0 || tabIndex >= s_Tabs.size())
			return;

		if (s_Tabs[tabIndex].Path.empty())
		{
			// TODO: File dialog for save
			NB_CORE_WARN("Cannot save: No file path specified");
			return;
		}

		std::ofstream file(s_Tabs[tabIndex].Path);
		if (file.is_open())
		{
			file << s_Tabs[tabIndex].Content;
			file.close();
			s_Tabs[tabIndex].IsDirty = false;
			NB_CORE_INFO("Script saved: {0}", s_Tabs[tabIndex].Path);
		}
		else
		{
			NB_CORE_ERROR("Failed to save script: {0}", s_Tabs[tabIndex].Path);
		}
	}

	void ScriptEditor::ReloadScript()
	{
		if (s_ActiveTab < 0 || s_ActiveTab >= s_Tabs.size())
			return;

		if (s_Tabs[s_ActiveTab].Path.empty())
		{
			NB_CORE_WARN("No script file to reload");
			return;
		}

		// Save first if dirty
		if (s_Tabs[s_ActiveTab].IsDirty)
			SaveScript(s_ActiveTab);

		// C# scripts are compiled by the build system, not reloaded at runtime
		NB_CORE_INFO("Script saved: {0}. Rebuild C# assemblies to apply changes.", s_Tabs[s_ActiveTab].Path);
	}

}
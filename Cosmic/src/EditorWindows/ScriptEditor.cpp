#include "nbpch.h"
#include "ScriptEditor.h"

#include "Nebula/ImGui/NebulaGui.h"
#include <Nebula/Log.h>
#include <Nebula/Scripting/LuaScriptEngine.h>
#include <imgui.h>

#include <fstream>
#include <sstream>

using namespace Nebula;

namespace Cosmic {

	std::string ScriptEditor::s_CurrentScriptPath;
	std::string ScriptEditor::s_ScriptContent;
	bool ScriptEditor::s_IsDirty = false;
	bool ScriptEditor::s_Visible = false;

	void ScriptEditor::OnImGuiRender()
	{
		if (!s_Visible) return;

		ImGui::SetNextWindowSize(ImVec2(250, 250), ImGuiCond_FirstUseEver);
		NebulaGui::Begin("Script Editor");

		// File menu
		if (NebulaGui::BeginMenuBar())
		{
			if (NebulaGui::BeginMenu("File"))
			{
			if (ImGui::MenuItem("New Script"))
			{
				s_CurrentScriptPath = "";
				s_ScriptContent = "-- New Lua Script\n-- Available APIs: Transform, Input, Log, Key, Mouse\n\nfunction OnCreate(entity)\n    Log.Info(\"Script created!\")\nend\n\nfunction OnUpdate(entity, deltaTime)\n    -- Update logic here\n    -- Example: Move with WASD\n    -- if Input.IsKeyPressed(Key.W) then\n    --     local pos = Transform.GetPosition(entity)\n    --     Transform.SetPosition(entity, pos.x, pos.y + 5 * deltaTime, pos.z)\n    -- end\nend\n\nfunction OnDestroy(entity)\n    -- Cleanup logic here\nend\n";
				s_IsDirty = true;
			}				if (ImGui::MenuItem("Open Script"))
				{
					// TODO: File dialog
				}

				if (ImGui::MenuItem("Save Script", nullptr, false, s_IsDirty))
				{
					SaveScript();
				}

			if (ImGui::MenuItem("Compile Script"))
			{
				ReloadScript();
			}				NebulaGui::EndMenu();
			}
			NebulaGui::EndMenuBar();
		}

		// Text editor
		if (ImGui::InputTextMultiline("##ScriptContent", &s_ScriptContent[0], s_ScriptContent.size() + 1024, ImVec2(-1, -1)))
		{
			s_IsDirty = true;
		}

		NebulaGui::End();
	}

	void ScriptEditor::LoadScript(const std::string& path)
	{
		std::ifstream file(path);
		if (file.is_open())
		{
			std::stringstream buffer;
			buffer << file.rdbuf();
			s_ScriptContent = buffer.str();
			s_CurrentScriptPath = path;
			s_IsDirty = false;
			file.close();
		}
		else
		{
			NB_CORE_ERROR("Failed to load script: {0}", path);
		}
	}

	void ScriptEditor::SaveScript()
	{
		if (s_CurrentScriptPath.empty())
		{
			// TODO: File dialog for save
			return;
		}

		std::ofstream file(s_CurrentScriptPath);
		if (file.is_open())
		{
			file << s_ScriptContent;
			file.close();
			s_IsDirty = false;
			NB_CORE_INFO("Script saved: {0}", s_CurrentScriptPath);
		}
		else
		{
			NB_CORE_ERROR("Failed to save script: {0}", s_CurrentScriptPath);
		}
	}

	void ScriptEditor::ReloadScript()
	{
		if (s_CurrentScriptPath.empty())
		{
			NB_CORE_WARN("No script file to reload");
			return;
		}

		// Reload the Lua script
		if (LuaScriptEngine::ReloadScript(s_CurrentScriptPath))
		{
			NB_CORE_INFO("Script reloaded: {0}", s_CurrentScriptPath);
		}
		else
		{
			NB_CORE_ERROR("Failed to reload script: {0}", s_CurrentScriptPath);
			NB_CORE_ERROR("Error: {0}", LuaScriptEngine::GetLastError());
		}
	}

}
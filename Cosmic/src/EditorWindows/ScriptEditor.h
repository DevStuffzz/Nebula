#pragma once

#include <string>

namespace Cosmic {

	class ScriptEditor
	{
	public:
		static void OnImGuiRender();
		static void ToggleVisibility() { s_Visible = !s_Visible; }

	private:
		static std::string s_CurrentScriptPath;
		static std::string s_ScriptContent;
		static bool s_IsDirty;
		static bool s_Visible;

		static void LoadScript(const std::string& path);
		static void SaveScript();
		static void ReloadScript();
	};

}
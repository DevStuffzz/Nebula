#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Cosmic {

	struct ScriptTab
	{
		std::string Path;
		std::string Content;
		bool IsDirty = false;
	};

	class ScriptEditor
	{
	public:
		static void OnImGuiRender();
		static void LoadScript(const std::string& path);
		static void SetRuntimeMode(bool runtime) { s_RuntimeMode = runtime; }

	private:
		static std::vector<ScriptTab> s_Tabs;
		static int s_ActiveTab;
		static bool s_RuntimeMode;

		static void SaveScript(int tabIndex = -1);
		static void ReloadScript();
		static void CloseTab(int tabIndex);
		static void NewScript();
	};

}
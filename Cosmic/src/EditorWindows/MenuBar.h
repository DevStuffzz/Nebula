#pragma once

#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Scene/Scene.h"
#include <functional>

namespace Cosmic {

	class MenuBar
	{
	public:
		using NewSceneCallback = std::function<void()>;
		using SaveSceneCallback = std::function<void()>;
		using LoadSceneCallback = std::function<void()>;
		using ExitCallback = std::function<void()>;
		using RunCallback = std::function<void()>;
		using SceneListCallback = std::function<void()>;

		static void SetCallbacks(
			NewSceneCallback newSceneCallback,
			SaveSceneCallback saveSceneCallback,
			LoadSceneCallback loadSceneCallback,
			ExitCallback exitCallback,
			RunCallback runCallback,
			SceneListCallback sceneListCallback)
		{
			s_NewSceneCallback = newSceneCallback;
			s_SaveSceneCallback = saveSceneCallback;
			s_LoadSceneCallback = loadSceneCallback;
			s_ExitCallback = exitCallback;
			s_RunCallback = runCallback;
			s_SceneListCallback = sceneListCallback;
		}

		static void SetRuntimeMode(bool runtime) { s_RuntimeMode = runtime; }

		static void OnImGuiRender()
		{
			if (Nebula::NebulaGui::BeginMenuBar())
			{
				if (Nebula::NebulaGui::BeginMenu("File"))
				{
					if (Nebula::NebulaGui::MenuItem("New Scene", "Ctrl+N"))
					{
						if (s_NewSceneCallback)
							s_NewSceneCallback();
					}

					if (Nebula::NebulaGui::MenuItem("Save Scene", "Ctrl+S"))
					{
						if (s_SaveSceneCallback)
							s_SaveSceneCallback();
					}

					if (Nebula::NebulaGui::MenuItem("Load Scene", "Ctrl+O"))
					{
						if (s_LoadSceneCallback)
							s_LoadSceneCallback();
					}

					Nebula::NebulaGui::Separator();

					if (Nebula::NebulaGui::MenuItem(s_RuntimeMode ? "Stop" : "Run", "F5"))
					{
						if (s_RunCallback)
							s_RunCallback();
					}

					Nebula::NebulaGui::Separator();

					if (Nebula::NebulaGui::MenuItem("Exit"))
					{
						if (s_ExitCallback)
							s_ExitCallback();
					}

					Nebula::NebulaGui::EndMenu();
				}

				if (Nebula::NebulaGui::BeginMenu("Edit"))
				{
					if (Nebula::NebulaGui::MenuItem("Undo", "Ctrl+Z")) {}
					if (Nebula::NebulaGui::MenuItem("Redo", "Ctrl+Y")) {}
					Nebula::NebulaGui::EndMenu();
				}

				if (Nebula::NebulaGui::BeginMenu("View"))
				{
					if (Nebula::NebulaGui::MenuItem("Scene List"))
					{
						if (s_SceneListCallback)
							s_SceneListCallback();
					}
					if (Nebula::NebulaGui::MenuItem("Reset Layout")) {}
					Nebula::NebulaGui::EndMenu();
				}

				Nebula::NebulaGui::EndMenuBar();
			}
		}

	private:
		static NewSceneCallback s_NewSceneCallback;
		static SaveSceneCallback s_SaveSceneCallback;
		static LoadSceneCallback s_LoadSceneCallback;
		static ExitCallback s_ExitCallback;
		static RunCallback s_RunCallback;
		static SceneListCallback s_SceneListCallback;
		static bool s_RuntimeMode;
	};

inline MenuBar::NewSceneCallback MenuBar::s_NewSceneCallback = nullptr;
inline MenuBar::SaveSceneCallback MenuBar::s_SaveSceneCallback = nullptr;
inline MenuBar::LoadSceneCallback MenuBar::s_LoadSceneCallback = nullptr;
inline MenuBar::ExitCallback MenuBar::s_ExitCallback = nullptr;
inline MenuBar::RunCallback MenuBar::s_RunCallback = nullptr;
inline MenuBar::SceneListCallback MenuBar::s_SceneListCallback = nullptr;
inline bool MenuBar::s_RuntimeMode = false;
}

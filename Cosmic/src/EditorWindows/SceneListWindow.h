#pragma once

#include <string>

namespace Cosmic {

	class SceneListWindow
	{
	public:
		SceneListWindow() = default;

		void OnImGuiRender();
		void SetOpen(bool open) { m_IsOpen = open; }
		bool IsOpen() const { return m_IsOpen; }

	private:
		void AddSceneDialog();
		void RemoveScene(size_t index);
		void MoveSceneUp(size_t index);
		void MoveSceneDown(size_t index);

	private:
		bool m_IsOpen = false;
		std::string m_SceneToAdd;
	};

}

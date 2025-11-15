#pragma once

#include "Nebula/ImGui/NebulaGui.h"
#include <filesystem>
#include <string>

namespace Cosmic {

	class ContentBrowser
	{
	public:
		static void SetContentPath(const std::filesystem::path& path)
		{
			s_BaseDirectory = path;
			s_CurrentDirectory = s_BaseDirectory;
		}

		static void OnImGuiRender()
		{
			Nebula::NebulaGui::Begin("Content Browser");

			if (s_CurrentDirectory != s_BaseDirectory)
			{
				if (Nebula::NebulaGui::Button("<-"))
				{
					s_CurrentDirectory = s_CurrentDirectory.parent_path();
				}
			}

			Nebula::NebulaGui::Text("Current Path: %s", s_CurrentDirectory.string().c_str());
			Nebula::NebulaGui::Separator();

			float padding = 16.0f;
			float thumbnailSize = 64.0f;
			float cellSize = thumbnailSize + padding;

			float panelWidth = Nebula::NebulaGui::GetContentRegionAvail().x;
			int columnCount = (int)(panelWidth / cellSize);
			if (columnCount < 1) columnCount = 1;

			Nebula::NebulaGui::Columns(columnCount, nullptr, false);

			if (std::filesystem::exists(s_CurrentDirectory))
			{
				for (auto& directoryEntry : std::filesystem::directory_iterator(s_CurrentDirectory))
				{
					const auto& path = directoryEntry.path();
					auto relativePath = std::filesystem::relative(path, s_BaseDirectory);
					std::string filenameString = relativePath.filename().string();

					Nebula::NebulaGui::PushID(filenameString.c_str());

					bool isDirectory = directoryEntry.is_directory();
					
					// Display icon (simplified - just text for now)
					if (Nebula::NebulaGui::Button(isDirectory ? "[D]" : "[F]", glm::vec2(thumbnailSize, thumbnailSize)))
					{
						if (isDirectory)
						{
							s_CurrentDirectory /= path.filename();
						}
					}

					if (Nebula::NebulaGui::BeginDragDropSource())
					{
						std::string pathStr = path.string();
						Nebula::NebulaGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", pathStr.c_str(), pathStr.size() + 1);
						Nebula::NebulaGui::EndDragDropSource();
					}

					Nebula::NebulaGui::TextWrapped(filenameString.c_str());

					Nebula::NebulaGui::NextColumn();

					Nebula::NebulaGui::PopID();
				}
			}

			Nebula::NebulaGui::Columns(1);

			Nebula::NebulaGui::End();
		}

	private:
		static std::filesystem::path s_BaseDirectory;
		static std::filesystem::path s_CurrentDirectory;
	};

	inline std::filesystem::path ContentBrowser::s_BaseDirectory = "";
	inline std::filesystem::path ContentBrowser::s_CurrentDirectory = "";
}

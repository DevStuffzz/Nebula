#pragma once

#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Asset/AssetManager.h"
#include "Nebula/Renderer/Texture.h"
#include <filesystem>
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>

namespace Cosmic {

	class ContentBrowser
	{
	public:
		using SceneLoadCallback = std::function<void(const std::string&)>;

		static void Initialize()
		{
			// Load editor icons
			s_DirectoryIcon = std::shared_ptr<Nebula::Texture2D>(Nebula::Texture2D::Create("assets/editor/icons/directory.png"));
			s_AudioIcon = std::shared_ptr<Nebula::Texture2D>(Nebula::Texture2D::Create("assets/editor/icons/audio.png"));
			s_MaterialIcon = std::shared_ptr<Nebula::Texture2D>(Nebula::Texture2D::Create("assets/editor/icons/material.png"));
			s_SceneIcon = std::shared_ptr<Nebula::Texture2D>(Nebula::Texture2D::Create("assets/editor/icons/scene.png"));
			s_ScriptIcon = std::shared_ptr<Nebula::Texture2D>(Nebula::Texture2D::Create("assets/editor/icons/script.png"));
			s_ShaderIcon = std::shared_ptr<Nebula::Texture2D>(Nebula::Texture2D::Create("assets/editor/icons/shader.png"));
		}

		static void SetContentPath(const std::filesystem::path& path)
		{
			s_BaseDirectory = path;
			s_CurrentDirectory = s_BaseDirectory;
		}

		static void SetSceneLoadCallback(SceneLoadCallback callback)
		{
			s_SceneLoadCallback = callback;
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

					// Skip .meta files
					if (path.extension() == ".meta")
						continue;

					Nebula::NebulaGui::PushID(filenameString.c_str());

					bool isDirectory = directoryEntry.is_directory();
					
					// Determine which icon/texture to display
					std::shared_ptr<Nebula::Texture2D> displayTexture = nullptr;
					Nebula::AssetType assetType = Nebula::AssetType::None;
					
					if (isDirectory)
					{
						displayTexture = s_DirectoryIcon;
					}
					else
					{
						assetType = Nebula::AssetManager::GetAssetTypeFromExtension(path.extension().string());
						switch (assetType)
						{
							case Nebula::AssetType::Texture2D:
								// For textures, load and display the actual texture
								{
									auto it = s_TextureCache.find(path.string());
									if (it != s_TextureCache.end())
									{
										displayTexture = it->second;
									}
									else
									{
										displayTexture = std::shared_ptr<Nebula::Texture2D>(Nebula::Texture2D::Create(path.string()));
										s_TextureCache[path.string()] = displayTexture;
									}
								}
								break;
							case Nebula::AssetType::Shader:    displayTexture = s_ShaderIcon; break;
							case Nebula::AssetType::Material:  displayTexture = s_MaterialIcon; break;
							case Nebula::AssetType::Scene:     displayTexture = s_SceneIcon; break;
							case Nebula::AssetType::Audio:     displayTexture = s_AudioIcon; break;
							case Nebula::AssetType::Script:    displayTexture = s_ScriptIcon; break;
							default:                           displayTexture = s_DirectoryIcon; break; // Use directory icon as fallback
						}
					}
					
					// Display the icon/texture as an image button
					bool clicked = false;
					if (displayTexture)
					{
						clicked = Nebula::NebulaGui::ImageButton("##icon", displayTexture->GetRendererID(), glm::vec2(thumbnailSize, thumbnailSize));
					}
					else
					{
						// Fallback to text button if no texture
						clicked = Nebula::NebulaGui::Button("[?]", glm::vec2(thumbnailSize, thumbnailSize));
					}

					if (clicked)
					if (clicked)
					{
						if (isDirectory)
						{
							s_CurrentDirectory /= path.filename();
						}
						else
						{
							// Handle double-click on files
							if (assetType == Nebula::AssetType::Scene && s_SceneLoadCallback)
							{
								s_SceneLoadCallback(path.string());
							}
						}
					}

					if (Nebula::NebulaGui::BeginDragDropSource())
					{
						std::string pathStr = path.string();
						std::string ext = path.extension().string();
						if (ext == ".obj")
							Nebula::NebulaGui::SetDragDropPayload("MESH_ASSET", pathStr.c_str(), pathStr.size() + 1);
						else
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
		static SceneLoadCallback s_SceneLoadCallback;
		
		// Editor icons
		static std::shared_ptr<Nebula::Texture2D> s_DirectoryIcon;
		static std::shared_ptr<Nebula::Texture2D> s_AudioIcon;
		static std::shared_ptr<Nebula::Texture2D> s_MaterialIcon;
		static std::shared_ptr<Nebula::Texture2D> s_SceneIcon;
		static std::shared_ptr<Nebula::Texture2D> s_ScriptIcon;
		static std::shared_ptr<Nebula::Texture2D> s_ShaderIcon;
		
		// Cache for texture thumbnails
		static std::unordered_map<std::string, std::shared_ptr<Nebula::Texture2D>> s_TextureCache;
	};

	inline std::filesystem::path ContentBrowser::s_BaseDirectory = "";
	inline std::filesystem::path ContentBrowser::s_CurrentDirectory = "";
	inline ContentBrowser::SceneLoadCallback ContentBrowser::s_SceneLoadCallback = nullptr;
	
	inline std::shared_ptr<Nebula::Texture2D> ContentBrowser::s_DirectoryIcon = nullptr;
	inline std::shared_ptr<Nebula::Texture2D> ContentBrowser::s_AudioIcon = nullptr;
	inline std::shared_ptr<Nebula::Texture2D> ContentBrowser::s_MaterialIcon = nullptr;
	inline std::shared_ptr<Nebula::Texture2D> ContentBrowser::s_SceneIcon = nullptr;
	inline std::shared_ptr<Nebula::Texture2D> ContentBrowser::s_ScriptIcon = nullptr;
	inline std::shared_ptr<Nebula::Texture2D> ContentBrowser::s_ShaderIcon = nullptr;
	inline std::unordered_map<std::string, std::shared_ptr<Nebula::Texture2D>> ContentBrowser::s_TextureCache;
}

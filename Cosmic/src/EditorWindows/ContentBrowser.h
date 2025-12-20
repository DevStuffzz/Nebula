#pragma once

#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Asset/AssetManager.h"
#include "Nebula/Renderer/Texture.h"
#include "Nebula/Log.h"
#include "ScriptEditor.h"
#include <filesystem>
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <fstream>

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
					{
						if (isDirectory)
						{
							s_CurrentDirectory /= path.filename();
						}
						else
						{
							// Handle click on files
							if (assetType == Nebula::AssetType::Scene && s_SceneLoadCallback)
							{
								s_SceneLoadCallback(path.string());
							}
							else if (assetType == Nebula::AssetType::Script)
							{
								// Load script in Script Editor
								ScriptEditor::LoadScript(path.string());
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

					// Right-click context menu on individual items
					if (Nebula::NebulaGui::BeginPopupContextItem())
					{
						if (Nebula::NebulaGui::MenuItem("Rename"))
						{
							s_SelectedPath = path;
							s_RenameBuffer = filenameString;
							s_IsRenaming = true;
						}
						Nebula::NebulaGui::EndPopup();
					}

					Nebula::NebulaGui::TextWrapped(filenameString.c_str());

					Nebula::NebulaGui::NextColumn();

					Nebula::NebulaGui::PopID();
				}
			}

			Nebula::NebulaGui::Columns(1);

			// Rename popup
			if (s_IsRenaming)
			{
				Nebula::NebulaGui::OpenPopup("Rename Asset");
				s_IsRenaming = false;
			}
			
			if (Nebula::NebulaGui::BeginPopup("Rename Asset"))
			{
				Nebula::NebulaGui::Text("Rename:");
				s_RenameBuffer.resize(256);
				if (Nebula::NebulaGui::InputText("##rename", &s_RenameBuffer[0], s_RenameBuffer.capacity()))
				{
					s_RenameBuffer = s_RenameBuffer.c_str(); // Trim
				}
				
				if (Nebula::NebulaGui::Button("OK"))
				{
					if (!s_RenameBuffer.empty())
					{
						RenameAsset(s_SelectedPath, s_RenameBuffer);
					}
					Nebula::NebulaGui::CloseCurrentPopup();
				}
				Nebula::NebulaGui::SameLine();
				if (Nebula::NebulaGui::Button("Cancel"))
				{
					Nebula::NebulaGui::CloseCurrentPopup();
				}
				
				Nebula::NebulaGui::EndPopup();
			}

			// Right-click context menu for creating new files
			if (Nebula::NebulaGui::BeginPopupContextWindow("ContentBrowserContext", 1, false))
			{
				if (Nebula::NebulaGui::MenuItem("Create Script"))
				{
					CreateNewScript();
				}
				Nebula::NebulaGui::EndPopup();
			}

			Nebula::NebulaGui::End();
		}

	private:
		static void CreateNewScript()
		{
			// Find a unique filename
			std::string baseName = "NewScript";
			std::string extension = ".lua";
			std::filesystem::path scriptPath = s_CurrentDirectory / (baseName + extension);
			
			int counter = 1;
			while (std::filesystem::exists(scriptPath))
			{
				scriptPath = s_CurrentDirectory / (baseName + std::to_string(counter) + extension);
				counter++;
			}

			// Create the script file with a basic template
			std::ofstream scriptFile(scriptPath);
			if (scriptFile.is_open())
			{
				scriptFile << "-- " << scriptPath.filename().string() << "\n\n";
				scriptFile << "function OnCreate()\n";
				scriptFile << "    -- Called when the entity is created\n";
				scriptFile << "end\n\n";
				scriptFile << "function OnUpdate(dt)\n";
				scriptFile << "    -- Called every frame\n";
				scriptFile << "    -- dt = delta time in seconds\n";
				scriptFile << "end\n";
				scriptFile.close();
				
				NB_CORE_INFO("Created new script: {0}", scriptPath.string());
			}
			else
			{
				NB_CORE_ERROR("Failed to create script file: {0}", scriptPath.string());
			}
		}
		
		static void RenameAsset(const std::filesystem::path& oldPath, const std::string& newName)
		{
			// Get the new path with the new name but same directory
			std::filesystem::path newPath = oldPath.parent_path() / newName;
			
			// Check if new path already exists
			if (std::filesystem::exists(newPath))
			{
				NB_CORE_ERROR("Cannot rename: {0} already exists", newPath.string());
				return;
			}
			
			// Get relative paths for scene file updates
			std::string oldRelPath = std::filesystem::relative(oldPath, s_BaseDirectory).string();
			std::string newRelPath = std::filesystem::relative(newPath, s_BaseDirectory).string();
			
			// Replace backslashes with forward slashes for consistent JSON paths
			std::replace(oldRelPath.begin(), oldRelPath.end(), '\\', '/');
			std::replace(newRelPath.begin(), newRelPath.end(), '\\', '/');
			
			// Update all references in scene files BEFORE renaming
			UpdateAssetReferencesInScenes(oldRelPath, newRelPath);
			
			// Perform the actual rename
			std::error_code ec;
			std::filesystem::rename(oldPath, newPath, ec);
			
			if (ec)
			{
				NB_CORE_ERROR("Failed to rename {0} to {1}: {2}", oldPath.string(), newPath.string(), ec.message());
			}
			else
			{
				NB_CORE_INFO("Renamed {0} to {1}", oldRelPath, newRelPath);
				NB_CORE_INFO("Updated all asset references in scene files");
			}
		}
		
		static void UpdateAssetReferencesInScenes(const std::string& oldPath, const std::string& newPath)
		{
			// Find all .nebscene files
			std::filesystem::path scenesDir = s_BaseDirectory / "scenes";
			
			if (!std::filesystem::exists(scenesDir))
				return;
			
			for (const auto& entry : std::filesystem::recursive_directory_iterator(scenesDir))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".nebscene")
				{
					UpdateReferencesInSceneFile(entry.path(), oldPath, newPath);
				}
			}
		}
		
		static void UpdateReferencesInSceneFile(const std::filesystem::path& sceneFile, const std::string& oldPath, const std::string& newPath)
		{
			// Read the entire scene file
			std::ifstream inFile(sceneFile);
			if (!inFile.is_open())
			{
				NB_CORE_ERROR("Failed to open scene file for reading: {0}", sceneFile.string());
				return;
			}
			
			std::stringstream buffer;
			buffer << inFile.rdbuf();
			std::string content = buffer.str();
			inFile.close();
			
			// Check if the old path exists in the content
			if (content.find(oldPath) == std::string::npos)
			{
				// No references to update
				return;
			}
			
			// Replace all occurrences of the old path with the new path
			size_t pos = 0;
			int replacementCount = 0;
			while ((pos = content.find(oldPath, pos)) != std::string::npos)
			{
				content.replace(pos, oldPath.length(), newPath);
				pos += newPath.length();
				replacementCount++;
			}
			
			// Write the updated content back to the file
			std::ofstream outFile(sceneFile);
			if (!outFile.is_open())
			{
				NB_CORE_ERROR("Failed to open scene file for writing: {0}", sceneFile.string());
				return;
			}
			
			outFile << content;
			outFile.close();
			
			if (replacementCount > 0)
			{
				NB_CORE_INFO("Updated {0} reference(s) in {1}", replacementCount, sceneFile.filename().string());
			}
		}

	private:
		static std::filesystem::path s_BaseDirectory;
		static std::filesystem::path s_CurrentDirectory;
		static SceneLoadCallback s_SceneLoadCallback;
		
		static std::filesystem::path s_SelectedPath;
		static std::string s_RenameBuffer;
		static bool s_IsRenaming;
		
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
	
	inline std::filesystem::path ContentBrowser::s_SelectedPath = "";
	inline std::string ContentBrowser::s_RenameBuffer = "";
	inline bool ContentBrowser::s_IsRenaming = false;
	
	inline std::shared_ptr<Nebula::Texture2D> ContentBrowser::s_DirectoryIcon = nullptr;
	inline std::shared_ptr<Nebula::Texture2D> ContentBrowser::s_AudioIcon = nullptr;
	inline std::shared_ptr<Nebula::Texture2D> ContentBrowser::s_MaterialIcon = nullptr;
	inline std::shared_ptr<Nebula::Texture2D> ContentBrowser::s_SceneIcon = nullptr;
	inline std::shared_ptr<Nebula::Texture2D> ContentBrowser::s_ScriptIcon = nullptr;
	inline std::shared_ptr<Nebula::Texture2D> ContentBrowser::s_ShaderIcon = nullptr;
	inline std::unordered_map<std::string, std::shared_ptr<Nebula::Texture2D>> ContentBrowser::s_TextureCache;
}

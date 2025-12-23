#pragma once
#pragma warning(disable: 4251)

// For use by Nebula Applications

#pragma once

#include "Nebula/Core/FileDialog.h"
#include "Nebula/Application.h"
#include "Nebula/Input.h"
#include "Nebula/MouseButtonCodes.h"

#include "Nebula/Keycodes.h"
#include "Nebula/Layer.h"
#include "Nebula/Log.h"
#include "Nebula/Timestep.h"

#include "Nebula/ImGui/ImGuiLayer.h"
#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/ImGui/NebulaGuizmo.h"


// Renderer
#include "Nebula/Renderer/Renderer.h"
#include "Nebula/Renderer/RenderCommand.h"
#include "Nebula/Renderer/Buffer.h"
#include "Nebula/Renderer/Shader.h"
#include "Nebula/Renderer/Texture.h"
#include "Nebula/Renderer/VertexArray.h"
#include "Nebula/Renderer/Camera.h"
#include "Nebula/Renderer/Material.h"
#include "Nebula/Renderer/Mesh.h"
#include "Nebula/Renderer/Framebuffer.h"
#include "Nebula/Renderer/LineRenderer.h"
#include "Nebula/Renderer/Skybox.h"

// Scene
#include "Nebula/Scene/Transform.h"
#include "Nebula/Scene/Entity.h"
#include "Nebula/Scene/Components.h"
#include "Nebula/Scene/Scene.h"
#include "Nebula/Scene/SceneSerializer.h"
#include "Nebula/Scene/SceneManager.h"

// Asset System
#include "Nebula/Asset/Asset.h"
#include "Nebula/Asset/AssetManager.h"
#include "Nebula/Asset/AssetManagerRegistry.h"
#include "Nebula/Asset/TextureImporter.h"
#include "Nebula/Asset/ShaderImporter.h"

// Scripting System
#include "Nebula/Scripting/ScriptEngine.h"

// Project System
#include "Nebula/Project/Project.h"

// Audio System
#include "Nebula/Audio/AudioEngine.h"
#include "Nebula/Audio/AudioClip.h"

// ------------- Entry Point
#include "Nebula/EntryPoint.h"
// -------------------------
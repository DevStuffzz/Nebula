# Scene Sharing Guide

This guide demonstrates how to use the same Scene in both client applications and the Cosmic editor.

## Overview

The `Nebula::Scene` class is part of the Nebula engine core and can be shared between:
- Client applications (like Sandbox)
- The Cosmic editor

## EditorLayer Scene Support

The `EditorLayer` now has built-in scene support with:

```cpp
// Set a scene from your application
editorLayer->SetActiveScene(myScene);

// Get the current active scene
Nebula::Scene* scene = editorLayer->GetActiveScene();
```

## Example 1: Using Scene in Sandbox App

See `Sandbox/src/SceneLayer.h` and `SceneLayer.cpp` for a complete example:

```cpp
#include <Nebula.h>
#include "SceneLayer.h"

class SandboxApp : public Nebula::Application
{
public:
    SandboxApp()
    {
        // Create and push the scene layer
        auto sceneLayer = new Sandbox::SceneLayer();
        PushLayer(sceneLayer);
    }
};

Nebula::Application* Nebula::CreateApplication()
{
    return new SandboxApp();
}
```

## Example 2: Sharing Scene with Editor

If you want to use the same scene in both Sandbox and Cosmic editor:

```cpp
// In your application
class SandboxApp : public Nebula::Application
{
public:
    SandboxApp()
    {
        // Create scene layer
        m_SceneLayer = new Sandbox::SceneLayer();
        PushLayer(m_SceneLayer);
        
        // If you also want editor support
        #ifdef ENABLE_EDITOR
        m_EditorLayer = new Cosmic::EditorLayer();
        m_EditorLayer->SetActiveScene(m_SceneLayer->GetScene());
        PushOverlay(m_EditorLayer);
        #endif
    }
    
private:
    Sandbox::SceneLayer* m_SceneLayer;
    #ifdef ENABLE_EDITOR
    Cosmic::EditorLayer* m_EditorLayer;
    #endif
};
```

## Example 3: Creating Entities in a Scene

```cpp
void SetupScene(Nebula::Scene* scene)
{
    // Create an entity
    auto entity = scene->CreateEntity("My Entity");
    
    // Modify its transform (all entities have this by default)
    auto& transform = entity.GetComponent<Nebula::TransformComponent>();
    transform.Position = glm::vec3(0.0f, 1.0f, 0.0f);
    transform.Rotation = glm::vec3(0.0f, 45.0f, 0.0f);
    transform.Scale = glm::vec3(1.0f);
    
    // Add a mesh renderer component
    auto& meshRenderer = entity.AddComponent<Nebula::MeshRendererComponent>();
    meshRenderer.Mesh = Nebula::Mesh::CreateCube();
    meshRenderer.Material = myMaterial;
}
```

## Example 4: Scene Update Loop

The scene needs to be updated each frame:

```cpp
void OnUpdate(Nebula::Timestep ts) override
{
    // Update scene systems (physics, scripts, etc.)
    if (m_Scene)
        m_Scene->OnUpdate(ts);
}
```

And rendered (this happens automatically in EditorLayer):

```cpp
// Rendering is handled by Scene::OnRender()
// which iterates through all entities with MeshRendererComponent
m_Scene->OnRender();
```

## Benefits of This Approach

1. **Single Source of Truth**: One scene can be edited and viewed in multiple contexts
2. **Reusability**: Scene data structures work identically in runtime and editor
3. **Flexibility**: You can choose to use scenes independently or share them
4. **Consistency**: Same entity-component system across all applications

## Scene Lifecycle

```
Create Scene → Add Entities → Update (each frame) → Render (each frame) → Destroy
```

The EditorLayer automatically creates a default scene if none is provided, but you can override it with `SetActiveScene()`.

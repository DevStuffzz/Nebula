# Scene Serialization System

## Overview

The Nebula Scene Serialization system provides JSON-based scene persistence with the `.nebscene` file extension. It supports saving and loading entire scenes including all entities and their components.

## File Format

Scene files use the `.nebscene` extension and contain human-readable JSON data:

```json
{
    "Scene": {
        "Name": "MyScene",
        "Version": "1.0"
    },
    "Entities": [
        {
            "ID": 123456,
            "TagComponent": {
                "Tag": "Player"
            },
            "TransformComponent": {
                "Position": [0.0, 1.0, 0.0],
                "Rotation": [0.0, 45.0, 0.0],
                "Scale": [1.0, 1.0, 1.0]
            },
            "MeshRendererComponent": {
                "HasMesh": true,
                "HasMaterial": true,
                "Material": {
                    "Color": [1.0, 0.5, 0.2, 1.0],
                    "Metallic": 0.0,
                    "Roughness": 0.5,
                    "UseTexture": 1,
                    "HasTexture": true
                }
            }
        }
    ]
}
```

## Usage

### Basic Scene Serialization

```cpp
#include <Nebula/Scene/Scene.h>
#include <Nebula/Scene/SceneSerializer.h>

// Create and populate a scene
auto scene = std::make_shared<Nebula::Scene>("My Game Scene");

auto entity = scene->CreateEntity("Player");
auto& transform = entity.GetComponent<Nebula::TransformComponent>();
transform.Position = glm::vec3(0.0f, 1.0f, 0.0f);

// Save the scene
Nebula::SceneSerializer serializer(scene.get());
serializer.Serialize("assets/scenes/MyScene.nebscene");
```

### Loading a Scene

```cpp
// Create an empty scene
auto scene = std::make_shared<Nebula::Scene>();

// Load from file
Nebula::SceneSerializer serializer(scene.get());
if (serializer.Deserialize("assets/scenes/MyScene.nebscene"))
{
    NB_INFO("Scene loaded successfully!");
}
```

### Serialize to String (Debug/Copy)

```cpp
Nebula::SceneSerializer serializer(scene.get());
std::string jsonString = serializer.SerializeToString();
NB_INFO("Scene JSON:\n{0}", jsonString);
```

## Serialized Components

### Always Present
- **TagComponent** - Entity name
- **TransformComponent** - Position, rotation, scale

### Optional Components
- **MeshRendererComponent**
  - Mesh presence flag
  - Material properties (color, metallic, roughness)
  - Texture usage flags
  
- **CameraComponent**
  - Primary camera flag
  - Projection type (Perspective/Orthographic)
  - Perspective properties (FOV, near/far clip)
  - Orthographic properties (size, near/far clip)

## Editor Integration

### Cosmic Editor

The editor provides menu options for scene management:

**File Menu:**
- `New Scene` (Ctrl+N) - Creates a blank scene
- `Save Scene` (Ctrl+S) - Saves current scene to `.nebscene` file
- `Load Scene` (Ctrl+O) - Loads scene from `.nebscene` file

### Implementation in EditorLayer

```cpp
void EditorLayer::SaveScene()
{
    if (m_CurrentScenePath.empty())
    {
        // Default save location
        std::filesystem::create_directories("assets/scenes");
        m_CurrentScenePath = "assets/scenes/" + m_ActiveScene->GetName() + ".nebscene";
    }

    Nebula::SceneSerializer serializer(m_ActiveScene.get());
    if (serializer.Serialize(m_CurrentScenePath))
    {
        NB_CORE_INFO("Scene saved to: {0}", m_CurrentScenePath);
    }
}

void EditorLayer::LoadScene()
{
    std::string filepath = "assets/scenes/MyScene.nebscene";
    
    if (!std::filesystem::exists(filepath))
    {
        NB_CORE_WARN("Scene file not found: {0}", filepath);
        return;
    }

    Nebula::SceneSerializer serializer(m_ActiveScene.get());
    if (serializer.Deserialize(filepath))
    {
        m_CurrentScenePath = filepath;
        SceneHierarchy::SetContext(m_ActiveScene);
    }
}
```

## Scene Class Methods

### New Methods for Serialization

```cpp
// Clear all entities from scene
void Scene::Clear();

// Set scene name
void Scene::SetName(const std::string& name);

// Get scene name
const std::string& Scene::GetName() const;
```

### Example Usage

```cpp
// Clear existing scene before loading
scene->Clear();

// Update scene name
scene->SetName("Level 1");
```

## Limitations & Future Improvements

### Current Limitations

1. **Mesh Serialization** - Meshes are recreated as cubes (default), not serialized by asset reference
2. **Texture Serialization** - Texture paths are not yet serialized (requires full asset system integration)
3. **Shader Serialization** - Shaders are loaded from hardcoded path `"assets/shaders/Basic.glsl"`
4. **No File Dialog** - Load path is currently hardcoded
5. **Material Shader** - Material shader reference is recreated, not preserved

### Planned Improvements

- [ ] **Asset Handle Integration** - Serialize meshes, textures, shaders by AssetHandle
- [ ] **File Dialog** - Native save/load file picker dialogs
- [ ] **Save As** - Allow saving to new location
- [ ] **Dirty Flag** - Track scene modifications and prompt before closing
- [ ] **Autosave** - Periodic automatic scene backup
- [ ] **Version Migration** - Handle loading older scene file versions
- [ ] **Incremental Save** - Only save modified entities
- [ ] **Binary Format** - Optional binary .nebscene format for faster loading
- [ ] **Compression** - Optional scene file compression

## File Organization

### Recommended Structure

```
assets/
  scenes/
    Level_01.nebscene
    Level_02.nebscene
    MainMenu.nebscene
  shaders/
    Basic.glsl
  textures/
    ...
  models/
    ...
```

### Content Browser Integration

Scene files appear in the Content Browser with `[SCN]` icon and can be:
- Dragged to load into editor
- Double-clicked to open
- Right-clicked for context menu options

## API Reference

### SceneSerializer Class

```cpp
class SceneSerializer
{
public:
    SceneSerializer(Scene* scene);

    // Serialize scene to .nebscene file
    bool Serialize(const std::string& filepath);
    
    // Deserialize scene from .nebscene file
    bool Deserialize(const std::string& filepath);
    
    // Serialize to JSON string (for debugging)
    std::string SerializeToString();
};
```

### Scene Methods

```cpp
class Scene
{
public:
    // Scene management
    void Clear();
    void SetName(const std::string& name);
    const std::string& GetName() const;
    
    // Entity management
    Entity CreateEntity(const std::string& name = "Entity");
    void DestroyEntity(Entity entity);
    std::vector<Entity> GetAllEntities() const;
};
```

## Error Handling

The serializer logs errors using the Nebula logging system:

```cpp
if (!serializer.Serialize(filepath))
{
    // Error already logged by serializer
    NB_CORE_ERROR("Failed to save scene!");
}
```

Common error messages:
- `"Failed to open file for writing"` - Directory doesn't exist or no write permissions
- `"Failed to parse scene JSON"` - Corrupted or invalid JSON file
- `"Failed to open scene file"` - File not found or no read permissions

## Testing the System

### Manual Test Workflow

1. Launch Cosmic editor
2. Create entities in the scene hierarchy
3. Add components (Transform, MeshRenderer)
4. Press Ctrl+S or use File → Save Scene
5. Close and reopen Cosmic
6. Press Ctrl+O or use File → Load Scene
7. Verify all entities and properties are restored

### Programmatic Test

```cpp
// Create and save a test scene
auto scene = std::make_shared<Nebula::Scene>("Test Scene");
auto entity = scene->CreateEntity("TestEntity");
auto& transform = entity.GetComponent<Nebula::TransformComponent>();
transform.Position = glm::vec3(1.0f, 2.0f, 3.0f);

Nebula::SceneSerializer serializer(scene.get());
serializer.Serialize("test.nebscene");

// Load and verify
auto loadedScene = std::make_shared<Nebula::Scene>();
Nebula::SceneSerializer loadSerializer(loadedScene.get());
loadSerializer.Deserialize("test.nebscene");

auto entities = loadedScene->GetAllEntities();
NEB_ASSERT(entities.size() == 1, "Should have 1 entity");
NEB_ASSERT(loadedScene->GetName() == "Test Scene", "Scene name should match");
```

## Dependencies

- **nlohmann/json** - JSON parsing and serialization library
- **std::filesystem** - File system operations (C++17)
- Scene and Component system
- Logging system

## Version History

**Version 1.0** (Current)
- Initial implementation
- Basic entity and component serialization
- TagComponent, TransformComponent, MeshRendererComponent, CameraComponent support
- JSON format with `.nebscene` extension
- Editor integration (Save/Load menu items)

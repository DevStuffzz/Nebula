# Asset Management System

## Overview

The Nebula Asset Management System provides a centralized, GUID-based approach to loading, caching, and managing engine assets. It supports automatic asset type detection, import pipelines, and metadata persistence.

## Core Concepts

### AssetHandle
A unique 64-bit identifier for each asset in the engine. Assets are referenced by handle rather than direct pointers.

```cpp
Nebula::AssetHandle handle = Nebula::AssetManager::GenerateAssetHandle();
```

### AssetType
Supported asset types:
- `Texture2D` - Images (.png, .jpg, .jpeg, .tga, .bmp)
- `Shader` - GLSL shaders (.glsl, .shader)
- `Mesh` - 3D models (.obj, .fbx, .gltf)
- `Material` - Material definitions (.mat)
- `Scene` - Scene files (.scene)
- `Audio` - Audio files (.wav, .mp3, .ogg)
- `Prefab` - Entity prefabs (.prefab)
- `Script` - Scripts (.lua, .cs)

### AssetMetadata
Stores information about an asset:
- `AssetHandle` - Unique identifier
- `AssetType` - Type of asset
- `FilePath` - Path to the asset file
- `IsLoaded` - Whether asset is currently in memory
- `IsMemoryAsset` - Whether asset was created at runtime

## Usage

### Initialization

The AssetManager is automatically initialized by the Application class:

```cpp
// In Application constructor
AssetManager::Init();
AssetManagerRegistry::RegisterImporters();
```

### Loading Assets

Load assets by file path (auto-detected type):

```cpp
// Load a texture
auto texture = Nebula::AssetManager::LoadAsset<Nebula::TextureAsset>("assets/textures/brick.png");
texture->Bind(0);

// Load a shader
auto shader = Nebula::AssetManager::LoadAsset<Nebula::ShaderAsset>("assets/shaders/Basic.glsl");
shader->Bind();
```

### Asset Handles

Get asset by handle:

```cpp
Nebula::AssetHandle handle = Nebula::AssetManager::ImportAsset("assets/textures/brick.png");
auto texture = Nebula::AssetManager::GetAsset<Nebula::TextureAsset>(handle);
```

### Creating Runtime Assets

Create assets programmatically without files:

```cpp
auto runtimeTexture = Nebula::AssetManager::CreateAsset<Nebula::TextureAsset>(
    /* constructor args */
);
```

### Asset Registry

Query asset information:

```cpp
// Check if asset is loaded
bool loaded = Nebula::AssetManager::IsAssetLoaded(handle);

// Get asset metadata
Nebula::AssetMetadata& metadata = Nebula::AssetManager::GetMetadata(handle);
NB_CORE_INFO("Asset type: {0}", Nebula::AssetTypeToString(metadata.Type));

// Get handle from file path
Nebula::AssetHandle handle = Nebula::AssetManager::GetAssetHandleFromPath("assets/textures/brick.png");
```

### Asset Importers

The system uses importers to convert file data into engine assets. Built-in importers:

- `TextureImporter` - Loads images using stb_image
- `ShaderImporter` - Parses and compiles GLSL shaders

Register custom importers:

```cpp
class MyCustomImporter : public Nebula::AssetImporter
{
public:
    virtual std::shared_ptr<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata) override
    {
        // Custom import logic
        return asset;
    }
    
    virtual bool CanImport(const std::string& extension) const override
    {
        return extension == ".custom";
    }
};

// Register the importer
Nebula::AssetManager::RegisterImporter(
    Nebula::AssetType::Custom,
    std::make_shared<MyCustomImporter>()
);
```

### Metadata Files (.meta)

Asset metadata is automatically saved to `.meta` files:

```
assets/textures/brick.png
assets/textures/brick.png.meta  # Generated metadata
```

Example `.meta` file content:
```
AssetHandle: 12345678901234567890
AssetType: Texture2D
FilePath: assets/textures/brick.png
IsMemoryAsset: false
```

## Content Browser Integration

The Content Browser automatically displays asset type icons:

- `[D]` - Directory
- `[IMG]` - Texture2D
- `[SHD]` - Shader
- `[3D]` - Mesh
- `[MAT]` - Material
- `[SCN]` - Scene
- `[AUD]` - Audio
- `[SCR]` - Script

Assets can be dragged from the Content Browser to property panels.

## Best Practices

1. **Use AssetHandles for references** - Store handles instead of raw pointers
2. **Load assets once** - The AssetManager caches loaded assets
3. **Unload unused assets** - Call `UnloadAsset()` to free memory
4. **Don't delete asset pointers** - AssetManager owns the memory
5. **Use typed asset classes** - `TextureAsset`, `ShaderAsset` provide type safety

## API Reference

### AssetManager Static Methods

```cpp
// Initialization
static void Init();
static void Shutdown();

// Loading
template<typename T> static std::shared_ptr<T> LoadAsset(const std::string& filepath);
template<typename T> static std::shared_ptr<T> GetAsset(AssetHandle handle);
static std::shared_ptr<Asset> LoadAssetFromPath(const std::string& filepath);

// Creation
template<typename T, typename... Args> static std::shared_ptr<T> CreateAsset(Args&&... args);

// Registration
static AssetHandle ImportAsset(const std::string& filepath);
static void RegisterAsset(const AssetMetadata& metadata, std::shared_ptr<Asset> asset);
static void UnloadAsset(AssetHandle handle);

// Queries
static AssetMetadata& GetMetadata(AssetHandle handle);
static AssetHandle GetAssetHandleFromPath(const std::string& filepath);
static bool IsAssetHandleValid(AssetHandle handle);
static bool IsAssetLoaded(AssetHandle handle);

// Utilities
static AssetType GetAssetTypeFromExtension(const std::string& extension);
static AssetHandle GenerateAssetHandle();
```

## Future Enhancements

- [ ] Model/Mesh importing with ASSIMP
- [ ] Asset hot-reloading
- [ ] Asset compression
- [ ] Texture atlasing
- [ ] Asset bundles for distribution
- [ ] Dependency tracking
- [ ] Asset validation

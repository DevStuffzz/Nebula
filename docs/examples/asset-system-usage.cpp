// Example: Using the Asset Management System in Nebula

#include <Nebula.h>

class ExampleLayer : public Nebula::Layer
{
public:
    ExampleLayer() : Layer("Example") {}

    virtual void OnAttach() override
    {
        // Example 1: Load a texture asset
        auto brickTexture = Nebula::AssetManager::LoadAsset<Nebula::TextureAsset>("assets/textures/brick.png");
        if (brickTexture && brickTexture->IsLoaded())
        {
            NB_INFO("Brick texture loaded! Size: {0}x{1}", 
                brickTexture->GetWidth(), 
                brickTexture->GetHeight());
            
            // Use the texture
            brickTexture->Bind(0);
        }

        // Example 2: Load a shader asset
        auto shader = Nebula::AssetManager::LoadAsset<Nebula::ShaderAsset>("assets/shaders/Basic.glsl");
        if (shader && shader->IsLoaded())
        {
            NB_INFO("Shader loaded from: {0}", shader->GetPath());
            shader->Bind();
        }

        // Example 3: Import an asset and get its handle
        Nebula::AssetHandle handle = Nebula::AssetManager::ImportAsset("assets/textures/checkerboard.png");
        if (handle.IsValid())
        {
            NB_INFO("Asset imported with handle: {0}", handle.Value);
            
            // Load it later using the handle
            auto texture = Nebula::AssetManager::GetAsset<Nebula::TextureAsset>(handle);
            
            // Get metadata
            Nebula::AssetMetadata& metadata = Nebula::AssetManager::GetMetadata(handle);
            NB_INFO("Asset type: {0}", Nebula::AssetTypeToString(metadata.Type));
            NB_INFO("Is loaded: {0}", metadata.IsLoaded ? "Yes" : "No");
        }

        // Example 4: Create a runtime asset
        // auto runtimeTexture = Nebula::AssetManager::CreateAsset<Nebula::TextureAsset>(
        //     "RuntimeTexture", 
        //     myTextureData
        // );

        // Example 5: Check if asset is already loaded
        Nebula::AssetHandle existingHandle = Nebula::AssetManager::GetAssetHandleFromPath("assets/textures/brick.png");
        if (Nebula::AssetManager::IsAssetLoaded(existingHandle))
        {
            NB_INFO("Brick texture is already loaded in memory");
        }

        // Example 6: Asset type detection
        std::string filepath = "assets/models/cube.obj";
        Nebula::AssetType type = Nebula::AssetManager::GetAssetTypeFromExtension(".obj");
        NB_INFO("File type for {0}: {1}", filepath, Nebula::AssetTypeToString(type));
    }

    virtual void OnDetach() override
    {
        // Assets are automatically managed by AssetManager
        // No need to manually delete
    }

    virtual void OnUpdate(Nebula::Timestep ts) override
    {
        // Use loaded assets in your rendering code
    }
};

// In your application class
class ExampleApp : public Nebula::Application
{
public:
    ExampleApp()
    {
        // AssetManager is already initialized by Application base class
        // Push your layer
        PushLayer(new ExampleLayer());
    }
};

Nebula::Application* Nebula::CreateApplication()
{
    return new ExampleApp();
}

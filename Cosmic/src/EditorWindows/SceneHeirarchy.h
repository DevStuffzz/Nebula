#pragma once

#include "Nebula/Scene/Scene.h"
#include "Nebula/Scene/Entity.h"
#include "Nebula/Scene/Components.h"
#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Log.h"
#include "Nebula/Renderer/Mesh.h"
#include "Nebula/Renderer/Shader.h"
#include "Nebula/Renderer/Material.h"
#include "Nebula/Log.h"
#include <string>
#include <glm/glm.hpp>

namespace Cosmic {

    class SceneHierarchy
    {
    public:
        // Set the scene context to display
        static void SetContext(const std::shared_ptr<Nebula::Scene>& scene)
        {
            s_Context = scene;
            s_SelectionContext = {};
        }

        // Get selected entity
        static Nebula::Entity GetSelectedEntity() { return s_SelectionContext; }

        // Render the hierarchy panel
        static void OnImGuiRender()
        {
            Nebula::NebulaGui::Begin("Scene Hierarchy");

            if (s_Context)
            {
                // Right-click on blank space
                if (Nebula::NebulaGui::BeginPopupContextWindow("HierarchyContextMenu", 1, false))
                {
                    if (Nebula::NebulaGui::MenuItem("Create Empty Entity"))
                    {
                        s_Context->CreateEntity("Entity");
                    }
                    if (Nebula::NebulaGui::MenuItem("Create Cube"))
                    {
                        CreatePrimitiveEntity("Cube", "Cube.obj");
                    }
                    if (Nebula::NebulaGui::MenuItem("Create Cylinder"))
                    {
                        CreatePrimitiveEntity("Cylinder", "Cylinder.obj");
                    }
                    if (Nebula::NebulaGui::MenuItem("Create Quad"))
                    {
                        CreatePrimitiveEntity("Quad", "Quad.obj");
                    }
                    if (Nebula::NebulaGui::MenuItem("Create Sphere"))
                    {
                        CreatePrimitiveEntity("Sphere", "Sphere.obj");
                    }
                    Nebula::NebulaGui::EndPopup();
                }

                // Check for F2 key press to start rename
                // List entities
                std::vector<Nebula::Entity> entities = s_Context->GetRootEntities();
                for (size_t i = 0; i < entities.size(); i++)
                {
                    DrawEntityNode(entities[i], i);
                }
            }

            Nebula::NebulaGui::End();
        }

    private:
        static void CreatePrimitiveEntity(const std::string& name, const std::string& objFile)
        {
            auto entity = s_Context->CreateEntity(name);
            auto& meshRenderer = entity.AddComponent<Nebula::MeshRendererComponent>();
            std::string meshPath = "Library/models/" + objFile;
            meshRenderer.Mesh = Nebula::Mesh::LoadOBJ(meshPath);
            meshRenderer.MeshSource = meshPath; // Store the source path
            Nebula::Shader* rawShader = Nebula::Shader::Create("Library/shaders/Basic.glsl");
            std::shared_ptr<Nebula::Shader> shader(rawShader);
            auto material = std::make_shared<Nebula::Material>(shader);
            material->SetFloat4("u_Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            material->SetInt("u_UseTexture", 0);
            meshRenderer.Material = material;
        }

        static void DrawEntityNode(Nebula::Entity entity, size_t entityIndex)
        {
            std::string name = "Unnamed Entity";

            if (entity.HasComponent<Nebula::TagComponent>())
            {
                auto& tag = entity.GetComponent<Nebula::TagComponent>();
                name = tag.Tag.empty() ? "Unnamed Entity" : tag.Tag;
            }

            bool isSelected = (s_SelectionContext == entity);
            bool isRenaming = (s_RenamingEntity == entity);
            
            // Check if entity has children
            bool hasChildren = entity.HasComponent<Nebula::HierarchyComponent>() && 
                             !entity.GetComponent<Nebula::HierarchyComponent>().Children.empty();
            
            // If renaming this entity, show input field
            if (isRenaming)
            {
                if (Nebula::NebulaGui::InputText("##rename", s_RenameBuffer, sizeof(s_RenameBuffer)))
                {
                    // Enter pressed - apply rename
                    if (entity.HasComponent<Nebula::TagComponent>())
                    {
                        auto& tag = entity.GetComponent<Nebula::TagComponent>();
                        tag.Tag = s_RenameBuffer;
                    }
                    s_RenamingEntity = {};
                }
            }
            else
            {
                // Only draw drop zone separator if not the first entity (entityIndex > 0)
                if (entityIndex > 0)
                {
                    // Create drop zone with separator centered in it
                    // Add spacing before separator
                    Nebula::NebulaGui::Dummy(glm::vec2(0.0f, 6.0f));
                    
                    // Draw separator
                    Nebula::NebulaGui::Separator();
                    
                    // Apply drop target to the separator
                    if (Nebula::NebulaGui::BeginDragDropTarget())
                    {
                        const void* payload = Nebula::NebulaGui::AcceptDragDropPayload("ENTITY_DRAG");
                        if (payload)
                        {
                            uint32_t sourceEntityID = *(const uint32_t*)payload;
                            Nebula::Entity sourceEntity = { (entt::entity)sourceEntityID, s_Context.get() };
                            if (sourceEntity != entity) // Don't reorder to same position
                            {
                                s_Context->ReorderEntity(sourceEntity, entityIndex);
                            }
                        }
                        Nebula::NebulaGui::EndDragDropTarget();
                    }
                    
                    // Add spacing after separator
                    Nebula::NebulaGui::Dummy(glm::vec2(0.0f, 6.0f));
                }
                
                // Draw entity - either as tree node if it has children, or as selectable
                bool nodeOpen = false;
                if (hasChildren)
                {
                    std::string label = name + "##" + std::to_string((uint32_t)entity);
                    nodeOpen = Nebula::NebulaGui::CollapsingHeader(label.c_str(), false);
                    
                    // Check if clicked for selection (CollapsingHeader returns true when clicked)
                    if (Nebula::NebulaGui::IsWindowHovered())
                    {
                        // Selection handled by context menu or direct click detection
                        // For now, we'll use the context menu to select
                    }
                }
                else
                {
                    if (Nebula::NebulaGui::Selectable(name.c_str(), isSelected))
                    {
                        s_SelectionContext = entity;
                    }
                }
                
                // Drag source for both reordering and parenting
                if (Nebula::NebulaGui::BeginDragDropSource())
                {
                    uint32_t entityID = (uint32_t)entity;
                    // Single payload type - action determined by drop target
                    Nebula::NebulaGui::SetDragDropPayload("ENTITY_DRAG", &entityID, sizeof(uint32_t));
                    Nebula::NebulaGui::Text("%s", name.c_str());
                    Nebula::NebulaGui::EndDragDropSource();
                }
                
                // Drop target for parenting (drop onto entity itself)
                if (Nebula::NebulaGui::BeginDragDropTarget())
                {
                    const void* payload = Nebula::NebulaGui::AcceptDragDropPayload("ENTITY_DRAG");
                    if (payload)
                    {
                        uint32_t sourceEntityID = *(const uint32_t*)payload;
                        Nebula::Entity sourceEntity = { (entt::entity)sourceEntityID, s_Context.get() };
                        if (sourceEntity != entity) // Don't parent to self
                        {
                            s_Context->SetParent(sourceEntity, entity);
                        }
                    }
                    Nebula::NebulaGui::EndDragDropTarget();
                }

                // Right-click context menu for entity
                if (Nebula::NebulaGui::BeginPopupContextItem())
                {
                    // Select entity when opening context menu
                    s_SelectionContext = entity;
                    
                    if (Nebula::NebulaGui::MenuItem("Rename"))
                    {
                        s_RenamingEntity = entity;
                        if (entity.HasComponent<Nebula::TagComponent>())
                        {
                            auto& tag = entity.GetComponent<Nebula::TagComponent>();
                            strncpy_s(s_RenameBuffer, tag.Tag.c_str(), sizeof(s_RenameBuffer) - 1);
                        }
                        else
                        {
                            s_RenameBuffer[0] = '\0';
                        }
                    }
                    
                    if (entity.HasComponent<Nebula::HierarchyComponent>() && 
                        entity.GetComponent<Nebula::HierarchyComponent>().Parent != 0)
                    {
                        if (Nebula::NebulaGui::MenuItem("Unparent"))
                        {
                            s_Context->RemoveParent(entity);
                        }
                    }
                    
                    if (Nebula::NebulaGui::MenuItem("Delete Entity"))
                    {
                        if (s_SelectionContext == entity)
                            s_SelectionContext = {};
                        if (s_RenamingEntity == entity)
                            s_RenamingEntity = {};
                        s_Context->DestroyEntity(entity);
                    }
                    Nebula::NebulaGui::EndPopup();
                }
                
                // Draw children if node is open
                if (hasChildren && nodeOpen)
                {
                    auto& hierarchy = entity.GetComponent<Nebula::HierarchyComponent>();
                    for (size_t i = 0; i < hierarchy.Children.size(); i++)
                    {
                        Nebula::Entity child = { (entt::entity)hierarchy.Children[i], s_Context.get() };
                        DrawEntityNode(child, i);
                    }
                }
            }
        }

    private:
        static std::shared_ptr<Nebula::Scene> s_Context;
        static Nebula::Entity s_SelectionContext;
        static Nebula::Entity s_RenamingEntity;
        static char s_RenameBuffer[256];
    };

    // Static variable definitions
    inline std::shared_ptr<Nebula::Scene> SceneHierarchy::s_Context = nullptr;
    inline Nebula::Entity SceneHierarchy::s_SelectionContext = {};
    inline Nebula::Entity SceneHierarchy::s_RenamingEntity = {};
    inline char SceneHierarchy::s_RenameBuffer[256] = "";
}

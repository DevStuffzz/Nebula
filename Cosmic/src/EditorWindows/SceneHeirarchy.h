#pragma once

#include "Nebula/Scene/Scene.h"
#include "Nebula/Scene/Entity.h"
#include "Nebula/Scene/Components.h"
#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Renderer/Mesh.h"
#include "Nebula/Renderer/Shader.h"
#include "Nebula/Renderer/Material.h"
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
                        CreateCubeEntity();
                    }
                    Nebula::NebulaGui::EndPopup();
                }

                // List entities
                std::vector<Nebula::Entity> entities = s_Context->GetAllEntities();
                for (const Nebula::Entity& entity : entities)
                {
                    DrawEntityNode(entity);
                }
            }

            Nebula::NebulaGui::End();
        }

    private:
        static void CreateCubeEntity()
        {
            auto entity = s_Context->CreateEntity("Cube");
            auto& meshRenderer = entity.AddComponent<Nebula::MeshRendererComponent>();
            
            meshRenderer.Mesh = Nebula::Mesh::CreateCube();
            
            Nebula::Shader* rawShader = Nebula::Shader::Create("assets/shaders/Basic.glsl");
            std::shared_ptr<Nebula::Shader> shader(rawShader);
            
            auto material = std::make_shared<Nebula::Material>(shader);
            material->SetFloat4("u_Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            material->SetInt("u_UseTexture", 0);
            
            meshRenderer.Material = material;
        }

        static void DrawEntityNode(Nebula::Entity entity)
        {
            std::string name = "Unnamed Entity";

            if (entity.HasComponent<Nebula::TagComponent>())
            {
                auto& tag = entity.GetComponent<Nebula::TagComponent>();
                name = tag.Tag;
            }

            bool isSelected = (s_SelectionContext == entity);
            
            if (Nebula::NebulaGui::Selectable(name.c_str(), isSelected))
            {
                s_SelectionContext = entity;
            }

            // Right-click context menu for entity
            if (Nebula::NebulaGui::BeginPopupContextItem())
            {
                if (Nebula::NebulaGui::MenuItem("Delete Entity"))
                {
                    if (s_SelectionContext == entity)
                        s_SelectionContext = {};
                    s_Context->DestroyEntity(entity);
                }
                Nebula::NebulaGui::EndPopup();
            }
        }

    private:
        static std::shared_ptr<Nebula::Scene> s_Context;
        static Nebula::Entity s_SelectionContext;
    };

    // Static variable definitions
    inline std::shared_ptr<Nebula::Scene> SceneHierarchy::s_Context = nullptr;
    inline Nebula::Entity SceneHierarchy::s_SelectionContext = {};
}

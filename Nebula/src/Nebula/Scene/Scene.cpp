#include "nbpch.h"
#include "Scene.h"
#include "Components.h"
#include "Nebula/Renderer/Renderer.h"
#include "Nebula/Application.h"

#include "Nebula/Application.h"

namespace Nebula {

	Scene::Scene(const std::string& name)
		: m_Name(name)
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		
		// Every entity gets a transform and tag component by default
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);
		
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdate(float deltaTime)
	{
		// Update systems here
		// For now, this is just a placeholder for future scripting/physics systems
	}

	void Scene::OnRender()
	{
		// Begin the scene with the application camera
		Renderer::BeginScene(Application::Get().GetCamera());

		// Render all entities with mesh renderer components
		auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
		for (auto entity : view)
		{
			auto [transform, meshRenderer] = view.get<TransformComponent, MeshRendererComponent>(entity);

			if (meshRenderer.Mesh && meshRenderer.Material)
			{
				Renderer::Submit(meshRenderer.Material, meshRenderer.Mesh, transform.GetTransform());
			}
		}

		// End the scene
		Renderer::EndScene();
	}

	std::vector<Entity> Scene::GetAllEntities() const
	{
		std::vector<Entity> entities;

		m_Registry.each([&](auto entityID) {
			entities.emplace_back(entityID, const_cast<Scene*>(this));
			});

		return entities;
	}



}

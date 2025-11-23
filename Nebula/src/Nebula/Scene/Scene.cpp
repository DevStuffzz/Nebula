#include "nbpch.h"
#include "Scene.h"
#include "Components.h"
#include "Scripts.h"
#include "Nebula/Renderer/Renderer.h"
#include "Nebula/Renderer/Material.h"
#include "Nebula/Application.h"

#include "Nebula/Application.h"

namespace Nebula {

	Scene::Scene(const std::string& name)
		: m_Name(name), m_GlobalIllumination(0.1f, 0.1f, 0.1f)
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

	void Scene::Clear()
	{
		m_Registry.clear();
	}

	void Scene::OnUpdate(float deltaTime)
	{
		// Update scripts
		auto view = m_Registry.view<ScriptComponent>();
		for (auto entity : view)
		{
			auto& scriptComp = view.get<ScriptComponent>(entity);
			auto key = std::make_pair(entity, scriptComp.ClassName);
			if (m_Scripts.find(key) == m_Scripts.end())
			{
				// Create script
				auto script = CreateScript(scriptComp.ClassName, { entity, this });
				if (script)
				{
					script->OnCreate();
					m_Scripts[key] = std::move(script);
				}
			}
			else
			{
				m_Scripts[key]->OnUpdate(deltaTime);
			}
		}

		// Update systems here
		// For now, this is just a placeholder for future scripting/physics systems
	}

	void Scene::OnRender()
	{
		// Begin the scene with the application camera
		Renderer::BeginScene(Application::Get().GetCamera());

		// Update scene-wide point lights list
		m_PointLights.clear();

		auto lightView = m_Registry.view<PointLightComponent, TransformComponent>();

		for (auto entity : lightView)
		{
			auto& light = lightView.get<PointLightComponent>(entity);
			auto& transform = lightView.get<TransformComponent>(entity);

			m_PointLights.push_back({
				transform.Position,  // use entity's world position
				light.Color,
				light.Intensity,
				light.Radius
				});
		}



		// Render all entities with mesh renderer components
		auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
		int numLights = (int)m_PointLights.size();
		for (auto entity : view)
		{
			auto [transform, meshRenderer] = view.get<TransformComponent, MeshRendererComponent>(entity);

			if (meshRenderer.Mesh && meshRenderer.Material)
			{
				std::shared_ptr<Nebula::Shader> shader = meshRenderer.Material->GetShader();
				shader->SetFloat3("u_GI", m_GlobalIllumination);
				shader->SetInt("u_NumPointLights", numLights);
				for (int i = 0; i < numLights && i < 4; ++i)
				{
					std::string idx = std::to_string(i);
					shader->SetFloat3("u_PointLights[" + idx + "].Position", m_PointLights[i].Position);
					shader->SetFloat3("u_PointLights[" + idx + "].Color", m_PointLights[i].Color);
					shader->SetFloat("u_PointLights[" + idx + "].Intensity", m_PointLights[i].Intensity);
					shader->SetFloat("u_PointLights[" + idx + "].Radius", m_PointLights[i].Radius);
				}
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

	std::unique_ptr<Script> Scene::CreateScript(const std::string& name, Entity entity)
	{
		if (name == "CameraController")
		{
			auto script = std::make_unique<CameraController>();
			script->m_Entity = entity;
			return script;
		}
		return nullptr;
	}

}

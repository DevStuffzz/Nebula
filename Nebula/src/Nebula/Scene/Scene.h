#pragma once

#include "Nebula/Core.h"
#include "Entity.h"
#include <entt/entt.hpp>
#include <string>

namespace Nebula {

	class NEBULA_API Scene
	{
	public:
		Scene(const std::string& name = "Untitled Scene");
		~Scene();

		Entity CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(Entity entity);

		void OnUpdate(float deltaTime);
		void OnRender();

		std::vector<Entity> GetAllEntities() const;

		const std::string& GetName() const { return m_Name; }		
		entt::registry& GetRegistry() { return m_Registry; }

	private:
		std::string m_Name;
		entt::registry m_Registry;

		friend class Entity;
		friend class SceneHierarchyPanel;
	};

}

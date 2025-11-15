#include "nbpch.h"
#include "Entity.h"
#include "Scene.h"

namespace Nebula {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

}

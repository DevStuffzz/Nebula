#pragma once

#include "Nebula/Core.h"
#include "Entity.h"
#include <entt/entt.hpp>
#include <string>
#include <unordered_map>
#include <memory>
#include <utility>
#include <functional>

#include "glm/glm.hpp"

struct PairHash {
    std::size_t operator()(const std::pair<entt::entity, std::string>& pair) const {
        return static_cast<std::size_t>(pair.first) ^ std::hash<std::string>()(pair.second);
    }
};

namespace Nebula {

	class Framebuffer;
	class Shader;

	class NEBULA_API Scene
	{
	public:
		Scene(const std::string& name = "Untitled Scene");
		~Scene();

		Entity CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(Entity entity);

		void OnUpdate(float deltaTime);
		void OnRender();

		void Clear(); // Clear all entities
		void SetName(const std::string& name) { m_Name = name; }

		std::vector<Entity> GetAllEntities() const;

		const std::string& GetName() const { return m_Name; }		
		entt::registry& GetRegistry() { return m_Registry; }

	private:
		std::string m_Name;
		entt::registry m_Registry;
		std::unordered_map<std::pair<entt::entity, std::string>, bool, PairHash> m_LuaScriptInitialized;

		// Scene-wide list of point lights
		struct PointLightData {
			glm::vec3 Position;
			glm::vec3 Color;
			float Intensity;
			float Radius;
		};
		std::vector<PointLightData> m_PointLights;

	// Scene-wide list of directional lights
	struct DirectionalLightData {
		glm::vec3 Direction;
		glm::vec3 Color;
		float Intensity;
		glm::mat4 LightSpaceMatrix;
		uint32_t ShadowMapTexture;
	};
	std::vector<DirectionalLightData> m_DirectionalLights;

	// Shadow mapping
	std::vector<std::shared_ptr<Framebuffer>> m_ShadowMapFramebuffers;
	std::shared_ptr<Shader> m_ShadowShader;
	void RenderShadowMaps();	private:
		glm::vec3 m_GlobalIllumination;

		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};

}

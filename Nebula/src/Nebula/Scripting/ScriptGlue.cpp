#include "nbpch.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "Nebula/Core.h"
#include "Nebula/Input.h"
#include "Nebula/Keycodes.h"

#include "Nebula/Scene/Scene.h"
#include "Nebula/Scene/Entity.h"
#include "Nebula/Scene/Components.h"

#include <mono/jit/jit.h>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

#include <glm/glm.hpp>

namespace Nebula {

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define NB_ADD_INTERNAL_CALL(Name) mono_add_internal_call("NebulaScriptCore.InternalCalls::" #Name, (void*)Name)

	// Console API
	static void Console_Log(MonoString* message)
	{
		char* cStr = mono_string_to_utf8(message);
		NB_CORE_INFO("{0}", cStr);
		mono_free(cStr);
	}

	static void Console_LogWarning(MonoString* message)
	{
		char* cStr = mono_string_to_utf8(message);
		NB_CORE_WARN("{0}", cStr);
		mono_free(cStr);
	}

	static void Console_LogError(MonoString* message)
	{
		char* cStr = mono_string_to_utf8(message);
		NB_CORE_ERROR("{0}", cStr);
		mono_free(cStr);
	}

	// Input API
	static bool Input_IsKeyPressed(int keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	static bool Input_IsKeyDown(int keycode)
	{
		return Input::IsKeyDown(keycode);
	}

	static bool Input_IsMouseButtonPressed(int button)
	{
		return Input::IsMouseButtonPressed(button);
	}

	static float Input_GetMouseX()
	{
		return Input::GetMouseX();
	}

	static float Input_GetMouseY()
	{
		return Input::GetMouseY();
	}

	// Entity API
	static bool Entity_HasComponent(uint32_t entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		NEB_CORE_ASSERT(entity, "Invalid entity!");

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		NEB_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end(), "Unknown component type!");
		
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	// Transform Component API
	static void TransformComponent_GetPosition(uint32_t entityID, glm::vec3* outPosition)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		NEB_CORE_ASSERT(entity, "Invalid entity!");

		*outPosition = entity.GetComponent<TransformComponent>().Position;
	}

	static void TransformComponent_SetPosition(uint32_t entityID, glm::vec3* position)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		NEB_CORE_ASSERT(entity, "Invalid entity!");

		entity.GetComponent<TransformComponent>().Position = *position;
	}

	static void TransformComponent_GetRotation(uint32_t entityID, glm::vec3* outRotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		NEB_CORE_ASSERT(entity, "Invalid entity!");

		*outRotation = entity.GetComponent<TransformComponent>().Rotation;
	}

	static void TransformComponent_SetRotation(uint32_t entityID, glm::vec3* rotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		NEB_CORE_ASSERT(entity, "Invalid entity!");

		entity.GetComponent<TransformComponent>().Rotation = *rotation;
	}

	static void TransformComponent_GetScale(uint32_t entityID, glm::vec3* outScale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		NEB_CORE_ASSERT(entity, "Invalid entity!");

		*outScale = entity.GetComponent<TransformComponent>().Scale;
	}

	static void TransformComponent_SetScale(uint32_t entityID, glm::vec3* scale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		NEB_CORE_ASSERT(entity, "Invalid entity!");

		entity.GetComponent<TransformComponent>().Scale = *scale;
	}

	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
		{
			std::string_view typeName = typeid(Component).name();
			size_t pos = typeName.find_last_of(':');
			std::string_view structName = typeName.substr(pos + 1);
			std::string managedTypename = fmt::format("NebulaScriptCore.{}", structName);

			MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
			if (!managedType)
			{
				NB_CORE_ERROR("Could not find component type {}", managedTypename);
				return;
			}
			s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
		}(), ...);
	}

	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}

	void ScriptGlue::RegisterFunctions()
	{
		NB_ADD_INTERNAL_CALL(Console_Log);
		NB_ADD_INTERNAL_CALL(Console_LogWarning);
		NB_ADD_INTERNAL_CALL(Console_LogError);

		NB_ADD_INTERNAL_CALL(Input_IsKeyPressed);
		NB_ADD_INTERNAL_CALL(Input_IsKeyDown);
		NB_ADD_INTERNAL_CALL(Input_IsMouseButtonPressed);
		NB_ADD_INTERNAL_CALL(Input_GetMouseX);
		NB_ADD_INTERNAL_CALL(Input_GetMouseY);

		NB_ADD_INTERNAL_CALL(Entity_HasComponent);
		NB_ADD_INTERNAL_CALL(TransformComponent_GetPosition);
		NB_ADD_INTERNAL_CALL(TransformComponent_SetPosition);
		NB_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		NB_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		NB_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		NB_ADD_INTERNAL_CALL(TransformComponent_SetScale);

		// Component registration moved to RegisterComponents()
		// Must be called after CoreAssembly is loaded
	}

	void ScriptGlue::RegisterComponents()
	{
		RegisterComponent<TransformComponent, MeshRendererComponent>();
	}

}

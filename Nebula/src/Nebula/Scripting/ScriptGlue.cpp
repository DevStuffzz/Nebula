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

	static bool Input_IsKeyReleased(int keycode)
	{
		// TODO: Implement key released detection
		return false;
	}

	static bool Input_IsMouseButtonPressed(int button)
	{
		return Input::IsMouseButtonPressed(button);
	}

	static bool Input_IsMouseButtonDown(int button)
	{
		// For now, use same as pressed (continuous check)
		return Input::IsMouseButtonPressed(button);
	}

	static bool Input_IsMouseButtonReleased(int button)
	{
		// TODO: Implement mouse button released detection
		return false;
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
		NB_CORE_INFO("GetPosition called for entity {}: ({}, {}, {})", entityID, outPosition->x, outPosition->y, outPosition->z);
	}

	static void TransformComponent_SetPosition(uint32_t entityID, glm::vec3* position)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		NEB_CORE_ASSERT(entity, "Invalid entity!");

		NB_CORE_INFO("SetPosition called for entity {}: ({}, {}, {})", entityID, position->x, position->y, position->z);
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

	// General Component API
	static bool Entity_GetComponent(uint32_t entityID, MonoReflectionType* componentType, MonoObject* outComponent)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		if (!entity)
			return false;

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		MonoClass* monoClass = mono_type_get_class(managedType);
		
		// Handle TransformComponent
		if (entity.HasComponent<TransformComponent>())
		{
			const char* className = mono_class_get_name(monoClass);
			if (strcmp(className, "TransformComponent") == 0)
			{
				auto& tc = entity.GetComponent<TransformComponent>();
				
				// Set fields
				MonoClassField* posField = mono_class_get_field_from_name(monoClass, "Position");
				MonoClassField* rotField = mono_class_get_field_from_name(monoClass, "Rotation");
				MonoClassField* scaleField = mono_class_get_field_from_name(monoClass, "Scale");
				
				if (posField) mono_field_set_value(outComponent, posField, &tc.Position);
				if (rotField) mono_field_set_value(outComponent, rotField, &tc.Rotation);
				if (scaleField) mono_field_set_value(outComponent, scaleField, &tc.Scale);
				
				return true;
			}
		}

		// Add more component types as needed
		return false;
	}

	static void Entity_AddComponent(uint32_t entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		NEB_CORE_ASSERT(entity, "Invalid entity!");

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		MonoClass* monoClass = mono_type_get_class(managedType);
		const char* className = mono_class_get_name(monoClass);

		// Handle different component types
		if (strcmp(className, "TransformComponent") == 0)
		{
			if (!entity.HasComponent<TransformComponent>())
				entity.AddComponent<TransformComponent>();
		}
		else if (strcmp(className, "MeshRendererComponent") == 0)
		{
			if (!entity.HasComponent<MeshRendererComponent>())
				entity.AddComponent<MeshRendererComponent>();
		}
		else if (strcmp(className, "CameraComponent") == 0)
		{
			if (!entity.HasComponent<CameraComponent>())
				entity.AddComponent<CameraComponent>();
		}
		// Add more component types as needed
	}

	static void Entity_RemoveComponent(uint32_t entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		NEB_CORE_ASSERT(entity, "Invalid entity!");

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		MonoClass* monoClass = mono_type_get_class(managedType);
		const char* className = mono_class_get_name(monoClass);

		// Handle different component types
		if (strcmp(className, "TransformComponent") == 0)
		{
			// Cannot remove TransformComponent - every entity needs one
			NB_CORE_WARN("Cannot remove TransformComponent from entity!");
		}
		else if (strcmp(className, "MeshRendererComponent") == 0)
		{
			if (entity.HasComponent<MeshRendererComponent>())
				entity.RemoveComponent<MeshRendererComponent>();
		}
		else if (strcmp(className, "CameraComponent") == 0)
		{
			if (entity.HasComponent<CameraComponent>())
				entity.RemoveComponent<CameraComponent>();
		}
		// Add more component types as needed
	}

	static MonoString* Entity_GetName(uint32_t entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		if (!entity || !entity.HasComponent<TagComponent>())
			return mono_string_new(mono_domain_get(), "");

		return mono_string_new(mono_domain_get(), entity.GetComponent<TagComponent>().Tag.c_str());
	}

	static void Entity_SetName(uint32_t entityID, MonoString* name)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		if (!entity || !entity.HasComponent<TagComponent>())
			return;

		char* cStr = mono_string_to_utf8(name);
		entity.GetComponent<TagComponent>().Tag = cStr;
		mono_free(cStr);
	}

	static MonoArray* Entity_GetAllEntitiesWithComponent(MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		MonoClass* monoClass = mono_type_get_class(managedType);
		const char* className = mono_class_get_name(monoClass);

		std::vector<uint32_t> entityIDs;

		// Find all entities with this component
		if (strcmp(className, "TransformComponent") == 0)
		{
			auto view = scene->GetRegistry().view<TransformComponent>();
			for (auto entity : view)
				entityIDs.push_back((uint32_t)entity);
		}
		else if (strcmp(className, "MeshRendererComponent") == 0)
		{
			auto view = scene->GetRegistry().view<MeshRendererComponent>();
			for (auto entity : view)
				entityIDs.push_back((uint32_t)entity);
		}
		else if (strcmp(className, "CameraComponent") == 0)
		{
			auto view = scene->GetRegistry().view<CameraComponent>();
			for (auto entity : view)
				entityIDs.push_back((uint32_t)entity);
		}
		else if (strcmp(className, "ScriptComponent") == 0)
		{
			auto view = scene->GetRegistry().view<ScriptComponent>();
			for (auto entity : view)
				entityIDs.push_back((uint32_t)entity);
		}
		// Add more component types as needed

		// Create managed array
		MonoArray* result = mono_array_new(mono_domain_get(), mono_get_uint32_class(), entityIDs.size());
		for (size_t i = 0; i < entityIDs.size(); i++)
		{
			mono_array_set(result, uint32_t, i, entityIDs[i]);
		}

		return result;
	}

	static uint32_t Entity_FindByName(MonoString* name)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");

		char* cStr = mono_string_to_utf8(name);
		std::string targetName(cStr);
		mono_free(cStr);

		// Search all entities with TagComponent
		auto view = scene->GetRegistry().view<TagComponent>();
		for (auto entity : view)
		{
			const auto& tag = view.get<TagComponent>(entity);
			if (tag.Tag == targetName)
				return (uint32_t)entity;
		}

		return 0; // Not found
	}

	static uint32_t Entity_Instantiate(uint32_t prefabID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity prefab{ (entt::entity)prefabID, scene };
		if (!prefab)
		{
			NB_CORE_ERROR("Invalid prefab entity!");
			return 0;
		}

		// Create new entity
		std::string name = "Clone";
		if (prefab.HasComponent<TagComponent>())
			name = prefab.GetComponent<TagComponent>().Tag + " (Clone)";

		Entity newEntity = scene->CreateEntity(name);

		// Copy TransformComponent
		if (prefab.HasComponent<TransformComponent>())
		{
			newEntity.GetComponent<TransformComponent>() = prefab.GetComponent<TransformComponent>();
		}

		// Copy MeshRendererComponent
		if (prefab.HasComponent<MeshRendererComponent>())
		{
			if (!newEntity.HasComponent<MeshRendererComponent>())
				newEntity.AddComponent<MeshRendererComponent>();
			newEntity.GetComponent<MeshRendererComponent>() = prefab.GetComponent<MeshRendererComponent>();
		}

		// Copy CameraComponent
		if (prefab.HasComponent<CameraComponent>())
		{
			if (!newEntity.HasComponent<CameraComponent>())
				newEntity.AddComponent<CameraComponent>();
			newEntity.GetComponent<CameraComponent>() = prefab.GetComponent<CameraComponent>();
		}

		// Copy ScriptComponent
		if (prefab.HasComponent<ScriptComponent>())
		{
			if (!newEntity.HasComponent<ScriptComponent>())
				newEntity.AddComponent<ScriptComponent>();
			newEntity.GetComponent<ScriptComponent>() = prefab.GetComponent<ScriptComponent>();
		}

		// Add more component types as needed

		return (uint32_t)newEntity;
	}

	static void Entity_Destroy(uint32_t entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		if (entity)
		{
			scene->DestroyEntity(entity);
		}
	}

	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
		{
			std::string_view typeName = typeid(Component).name();
			size_t pos = typeName.find_last_of(':');
			std::string_view structName = typeName.substr(pos + 1);
			std::string managedTypename = fmt::format("Nebula.{}", structName);

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
		NB_CORE_INFO("ScriptGlue: Registering internal calls...");
		
		// Register Console functions (Nebula.Console class)
		mono_add_internal_call("Nebula.Console::Log", (void*)Console_Log);
		mono_add_internal_call("Nebula.Console::LogWarning", (void*)Console_LogWarning);
		mono_add_internal_call("Nebula.Console::LogError", (void*)Console_LogError);
		NB_CORE_INFO("  Registered Nebula.Console functions");

		// Register Input functions (Nebula.Input class)
		mono_add_internal_call("Nebula.Input::IsKeyPressed", (void*)Input_IsKeyPressed);
		mono_add_internal_call("Nebula.Input::IsKeyDown", (void*)Input_IsKeyDown);
		mono_add_internal_call("Nebula.Input::IsKeyReleased", (void*)Input_IsKeyReleased);
		mono_add_internal_call("Nebula.Input::IsMouseButtonPressed", (void*)Input_IsMouseButtonPressed);
		mono_add_internal_call("Nebula.Input::IsMouseButtonDown", (void*)Input_IsMouseButtonDown);
		mono_add_internal_call("Nebula.Input::IsMouseButtonReleased", (void*)Input_IsMouseButtonReleased);
		mono_add_internal_call("Nebula.Input::GetMouseX", (void*)Input_GetMouseX);
		mono_add_internal_call("Nebula.Input::GetMouseY", (void*)Input_GetMouseY);
		NB_CORE_INFO("  Registered Nebula.Input functions");

		// Register Entity/Transform functions (Nebula.InternalCalls class)
		mono_add_internal_call("Nebula.InternalCalls::Entity_HasComponent", (void*)Entity_HasComponent);
		mono_add_internal_call("Nebula.InternalCalls::Entity_GetPosition", (void*)TransformComponent_GetPosition);
		mono_add_internal_call("Nebula.InternalCalls::Entity_SetPosition", (void*)TransformComponent_SetPosition);
		mono_add_internal_call("Nebula.InternalCalls::Entity_GetRotation", (void*)TransformComponent_GetRotation);
		mono_add_internal_call("Nebula.InternalCalls::Entity_SetRotation", (void*)TransformComponent_SetRotation);
		mono_add_internal_call("Nebula.InternalCalls::Entity_GetScale", (void*)TransformComponent_GetScale);
		mono_add_internal_call("Nebula.InternalCalls::Entity_SetScale", (void*)TransformComponent_SetScale);
		mono_add_internal_call("Nebula.InternalCalls::Entity_GetComponent", (void*)Entity_GetComponent);
		mono_add_internal_call("Nebula.InternalCalls::Entity_AddComponent", (void*)Entity_AddComponent);
		mono_add_internal_call("Nebula.InternalCalls::Entity_RemoveComponent", (void*)Entity_RemoveComponent);
		mono_add_internal_call("Nebula.InternalCalls::Entity_GetName", (void*)Entity_GetName);
		mono_add_internal_call("Nebula.InternalCalls::Entity_SetName", (void*)Entity_SetName);
		mono_add_internal_call("Nebula.InternalCalls::Entity_GetAllEntitiesWithComponent", (void*)Entity_GetAllEntitiesWithComponent);
		mono_add_internal_call("Nebula.InternalCalls::Entity_FindByName", (void*)Entity_FindByName);
		mono_add_internal_call("Nebula.InternalCalls::Entity_Instantiate", (void*)Entity_Instantiate);
		mono_add_internal_call("Nebula.InternalCalls::Entity_Destroy", (void*)Entity_Destroy);
		NB_CORE_INFO("  Registered Nebula.InternalCalls functions");
		
		NB_CORE_INFO("ScriptGlue: All internal calls registered successfully");

		// Component registration moved to RegisterComponents()
		// Must be called after CoreAssembly is loaded
	}

	void ScriptGlue::RegisterComponents()
	{
		RegisterComponent<TransformComponent, MeshRendererComponent, CameraComponent, TagComponent, ScriptComponent>();
	}

}

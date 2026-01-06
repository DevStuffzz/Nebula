#include "nbpch.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "Nebula/Core.h"
#include "Nebula/Log.h"
#include "Nebula/Input.h"
#include "Nebula/Keycodes.h"
#include "Nebula/Application.h"

#include "Nebula/Scene/Scene.h"
#include "Nebula/Scene/Entity.h"
#include "Nebula/Scene/Components.h"

#include <mono/jit/jit.h>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

#include <glm/glm.hpp>

namespace Nebula {

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;
	static std::unordered_map<uint32_t, float> s_DestroyDelayedEntities; // entityID -> timeRemaining
	static glm::vec2 s_LastMousePos = glm::vec2(0.0f);
	static glm::vec2 s_MouseDelta = glm::vec2(0.0f);
	static glm::vec2 s_MouseScrollDelta = glm::vec2(0.0f);

	// Log API
	static void Log_LogInfo(MonoString* message)
	{
		char* cStr = mono_string_to_utf8(message);
		NB_CORE_INFO("{0}", cStr);
		Nebula::Log::LogClientMessage(cStr, LogLevel::LOG_INFO);
		mono_free(cStr);
	}

	static void Log_LogWarning(MonoString* message)
	{
		char* cStr = mono_string_to_utf8(message);
		NB_CORE_WARN("{0}", cStr);
		Nebula::Log::LogClientMessage(cStr, LogLevel::LOG_WARN);
		mono_free(cStr);
	}

	static void Log_LogError(MonoString* message)
	{
		char* cStr = mono_string_to_utf8(message);
		NB_CORE_ERROR("{0}", cStr);
		Nebula::Log::LogClientMessage(cStr, LogLevel::LOG_ERROR);
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

	static void Input_GetMousePosition(glm::vec2* outPosition)
	{
		auto [x, y] = Input::GetMousePos();
		*outPosition = glm::vec2(x, y);
	}

	static void Input_GetMouseDelta(glm::vec2* outDelta)
	{
		*outDelta = s_MouseDelta;
	}

	static void Input_GetMouseScrollDelta(glm::vec2* outScroll)
	{
		*outScroll = s_MouseScrollDelta;
	}

	static float Input_GetAxisValue(MonoString* axisName)
	{
		char* cStr = mono_string_to_utf8(axisName);
		std::string axis(cStr);
		mono_free(cStr);

		// Implement common axes
		if (axis == "Horizontal")
		{
			float value = 0.0f;
			if (Input::IsKeyDown(NB_KEY_D) || Input::IsKeyDown(NB_KEY_RIGHT))
				value += 1.0f;
			if (Input::IsKeyDown(NB_KEY_A) || Input::IsKeyDown(NB_KEY_LEFT))
				value -= 1.0f;
			return value;
		}
		else if (axis == "Vertical")
		{
			float value = 0.0f;
			if (Input::IsKeyDown(NB_KEY_W) || Input::IsKeyDown(NB_KEY_UP))
				value += 1.0f;
			if (Input::IsKeyDown(NB_KEY_S) || Input::IsKeyDown(NB_KEY_DOWN))
				value -= 1.0f;
			return value;
		}

		return 0.0f;
	}

	static float Input_GetAxisRawValue(MonoString* axisName)
	{
		// For now, same as GetAxisValue (no smoothing implemented)
		return Input_GetAxisValue(axisName);
	}

	// Time API
	static float Time_GetDeltaTime()
	{
		return Application::Get().GetDeltaTime();
	}

	static float Time_GetTime()
	{
		return Application::Get().GetTime();
	}

	static float Time_GetTimeScale()
	{
		return Application::Get().GetTimeScale();
	}

	static void Time_SetTimeScale(float value)
	{
		Application::Get().SetTimeScale(value);
	}

	static float Time_GetFixedDeltaTime()
	{
		return 0.02f; // Default 50 FPS physics
	}

	static void Time_SetFixedDeltaTime(float value)
	{
		// TODO: Store and use in physics system
	}

	static float Time_GetUnscaledTime()
	{
		return Application::Get().GetUnscaledTime();
	}

	static float Time_GetUnscaledDeltaTime()
	{
		return Application::Get().GetUnscaledDeltaTime();
	}

	static int Time_GetFrameCount()
	{
		return Application::Get().GetFrameCount();
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
		else if (strcmp(className, "BoxColliderComponent") == 0)
		{
			if (!entity.HasComponent<BoxColliderComponent>())
				entity.AddComponent<BoxColliderComponent>();
		}
		else if (strcmp(className, "SphereColliderComponent") == 0)
		{
			if (!entity.HasComponent<SphereColliderComponent>())
				entity.AddComponent<SphereColliderComponent>();
		}
		else if (strcmp(className, "RigidBodyComponent") == 0)
		{
			if (!entity.HasComponent<RigidBodyComponent>())
				entity.AddComponent<RigidBodyComponent>();
		}
		else if (strcmp(className, "AudioSourceComponent") == 0)
		{
			if (!entity.HasComponent<AudioSourceComponent>())
				entity.AddComponent<AudioSourceComponent>();
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
		else if (strcmp(className, "BoxColliderComponent") == 0)
		{
			if (entity.HasComponent<BoxColliderComponent>())
				entity.RemoveComponent<BoxColliderComponent>();
		}
		else if (strcmp(className, "SphereColliderComponent") == 0)
		{
			if (entity.HasComponent<SphereColliderComponent>())
				entity.RemoveComponent<SphereColliderComponent>();
		}
		else if (strcmp(className, "RigidBodyComponent") == 0)
		{
			if (entity.HasComponent<RigidBodyComponent>())
				entity.RemoveComponent<RigidBodyComponent>();
		}
		else if (strcmp(className, "AudioSourceComponent") == 0)
		{
			if (entity.HasComponent<AudioSourceComponent>())
				entity.RemoveComponent<AudioSourceComponent>();
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
		else if (strcmp(className, "BoxColliderComponent") == 0)
		{
			auto view = scene->GetRegistry().view<BoxColliderComponent>();
			for (auto entity : view)
				entityIDs.push_back((uint32_t)entity);
		}
		else if (strcmp(className, "SphereColliderComponent") == 0)
		{
			auto view = scene->GetRegistry().view<SphereColliderComponent>();
			for (auto entity : view)
				entityIDs.push_back((uint32_t)entity);
		}
		else if (strcmp(className, "RigidBodyComponent") == 0)
		{
			auto view = scene->GetRegistry().view<RigidBodyComponent>();
			for (auto entity : view)
				entityIDs.push_back((uint32_t)entity);
		}
		else if (strcmp(className, "AudioSourceComponent") == 0)
		{
			auto view = scene->GetRegistry().view<AudioSourceComponent>();
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

	static void Entity_DestroyDelayed(uint32_t entityID, float delay)
	{
		s_DestroyDelayedEntities[entityID] = delay;
	}

	static MonoObject* Entity_GetScriptInstance(uint32_t entityID, MonoReflectionType* scriptType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		if (!entity || !entity.HasComponent<ScriptComponent>())
			return nullptr;

		Ref<ScriptInstance> instance = ScriptEngine::GetEntityScriptInstance(entityID);
		if (!instance)
			return nullptr;

		// Return the MonoObject
		return instance->GetManagedObject();
	}

	static MonoObject* Entity_GetScriptByName(uint32_t entityID, MonoString* className)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		if (!entity || !entity.HasComponent<ScriptComponent>())
			return nullptr;

		char* cStr = mono_string_to_utf8(className);
		std::string targetClassName(cStr);
		mono_free(cStr);

		auto& sc = entity.GetComponent<ScriptComponent>();
		if (sc.ClassName == targetClassName)
		{
			Ref<ScriptInstance> instance = ScriptEngine::GetEntityScriptInstance(entityID);
			if (!instance)
				return nullptr;

			return instance->GetManagedObject();
		}

		return nullptr;
	}

	static MonoString* Entity_GetTag(uint32_t entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		if (!entity || !entity.HasComponent<TagComponent>())
			return mono_string_new(mono_domain_get(), "Untagged");

		return mono_string_new(mono_domain_get(), entity.GetComponent<TagComponent>().Tag.c_str());
	}

	static void Entity_SetTag(uint32_t entityID, MonoString* tag)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");
		
		Entity entity{ (entt::entity)entityID, scene };
		if (!entity || !entity.HasComponent<TagComponent>())
			return;

		char* cStr = mono_string_to_utf8(tag);
		entity.GetComponent<TagComponent>().Tag = cStr;
		mono_free(cStr);
	}

	static uint32_t Entity_FindByTag(MonoString* tag)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");

		char* cStr = mono_string_to_utf8(tag);
		std::string targetTag(cStr);
		mono_free(cStr);

		auto view = scene->GetRegistry().view<TagComponent>();
		for (auto entity : view)
		{
			const auto& tagComp = view.get<TagComponent>(entity);
			if (tagComp.Tag == targetTag)
				return (uint32_t)entity;
		}

		return 0;
	}

	static MonoArray* Entity_FindAllByTag(MonoString* tag)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		NEB_CORE_ASSERT(scene, "No active scene!");

		char* cStr = mono_string_to_utf8(tag);
		std::string targetTag(cStr);
		mono_free(cStr);

		std::vector<uint32_t> entityIDs;
		auto view = scene->GetRegistry().view<TagComponent>();
		for (auto entity : view)
		{
			const auto& tagComp = view.get<TagComponent>(entity);
			if (tagComp.Tag == targetTag)
				entityIDs.push_back((uint32_t)entity);
		}

		MonoArray* result = mono_array_new(mono_domain_get(), mono_get_uint32_class(), entityIDs.size());
		for (size_t i = 0; i < entityIDs.size(); i++)
		{
			mono_array_set(result, uint32_t, i, entityIDs[i]);
		}

		return result;
	}

	static bool Entity_GetActiveSelf(uint32_t entityID)
	{
		// TODO: Add active state to entities
		return true;
	}

	static bool Entity_GetActiveInHierarchy(uint32_t entityID)
	{
		// TODO: Implement hierarchy active check
		return true;
	}

	static void Entity_SetActive(uint32_t entityID, bool active)
	{
		// TODO: Implement entity activation/deactivation
	}

	static uint32_t Entity_GetParent(uint32_t entityID)
	{
		// TODO: Implement hierarchy system
		return 0;
	}

	static void Entity_SetParent(uint32_t entityID, uint32_t parentID)
	{
		// TODO: Implement hierarchy system
	}

	static void Entity_SetParentWithTransform(uint32_t entityID, uint32_t parentID, bool worldPositionStays)
	{
		// TODO: Implement hierarchy system with transform preservation
	}

	static int Entity_GetChildCount(uint32_t entityID)
	{
		// TODO: Implement hierarchy system
		return 0;
	}

	static uint32_t Entity_GetChild(uint32_t entityID, int index)
	{
		// TODO: Implement hierarchy system
		return 0;
	}

	// Physics API
	static bool Physics_InternalRaycast(glm::vec3* origin, glm::vec3* direction, float maxDistance, void* outHit)
	{
		// TODO: Implement raycast with Bullet physics
		// For now, return false (no hit)
		return false;
	}

	static bool Physics_InternalSphereCast(glm::vec3* origin, float radius, glm::vec3* direction, float maxDistance, void* outHit)
	{
		// TODO: Implement sphere cast with Bullet physics
		return false;
	}

	static MonoArray* Physics_InternalOverlapSphere(glm::vec3* position, float radius)
	{
		// TODO: Implement overlap sphere with Bullet physics
		MonoArray* result = mono_array_new(mono_domain_get(), mono_get_uint32_class(), 0);
		return result;
	}

	static void Physics_GetGravity(glm::vec3* outGravity)
	{
		// TODO: Get from physics world
		*outGravity = glm::vec3(0.0f, -9.81f, 0.0f);
	}

	static void Physics_SetGravity(glm::vec3* gravity)
	{
		// TODO: Set physics world gravity
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
		
		// Register Log functions (Nebula.Log class)
		mono_add_internal_call("Nebula.Log::LogInfo", (void*)Log_LogInfo);
		mono_add_internal_call("Nebula.Log::LogWarning", (void*)Log_LogWarning);
		mono_add_internal_call("Nebula.Log::LogError", (void*)Log_LogError);
		NB_CORE_INFO("  Registered Nebula.Log functions");

		// Register Input functions (Nebula.Input class)
		mono_add_internal_call("Nebula.Input::IsKeyPressed", (void*)Input_IsKeyPressed);
		mono_add_internal_call("Nebula.Input::IsKeyDown", (void*)Input_IsKeyDown);
		mono_add_internal_call("Nebula.Input::IsKeyReleased", (void*)Input_IsKeyReleased);
		mono_add_internal_call("Nebula.Input::IsMouseButtonPressed", (void*)Input_IsMouseButtonPressed);
		mono_add_internal_call("Nebula.Input::IsMouseButtonDown", (void*)Input_IsMouseButtonDown);
		mono_add_internal_call("Nebula.Input::IsMouseButtonReleased", (void*)Input_IsMouseButtonReleased);
		mono_add_internal_call("Nebula.Input::GetMouseX", (void*)Input_GetMouseX);
		mono_add_internal_call("Nebula.Input::GetMouseY", (void*)Input_GetMouseY);
		mono_add_internal_call("Nebula.Input::GetMousePosition", (void*)Input_GetMousePosition);
		mono_add_internal_call("Nebula.Input::GetMouseDelta", (void*)Input_GetMouseDelta);
		mono_add_internal_call("Nebula.Input::GetMouseScrollDelta", (void*)Input_GetMouseScrollDelta);
		mono_add_internal_call("Nebula.Input::GetAxisValue", (void*)Input_GetAxisValue);
		mono_add_internal_call("Nebula.Input::GetAxisRawValue", (void*)Input_GetAxisRawValue);
		NB_CORE_INFO("  Registered Nebula.Input functions");

		// Register Time functions (Nebula.Time class)
		mono_add_internal_call("Nebula.Time::GetDeltaTime", (void*)Time_GetDeltaTime);
		mono_add_internal_call("Nebula.Time::GetTime", (void*)Time_GetTime);
		mono_add_internal_call("Nebula.Time::GetTimeScale", (void*)Time_GetTimeScale);
		mono_add_internal_call("Nebula.Time::SetTimeScale", (void*)Time_SetTimeScale);
		mono_add_internal_call("Nebula.Time::GetFixedDeltaTime", (void*)Time_GetFixedDeltaTime);
		mono_add_internal_call("Nebula.Time::SetFixedDeltaTime", (void*)Time_SetFixedDeltaTime);
		mono_add_internal_call("Nebula.Time::GetUnscaledTime", (void*)Time_GetUnscaledTime);
		mono_add_internal_call("Nebula.Time::GetUnscaledDeltaTime", (void*)Time_GetUnscaledDeltaTime);
		mono_add_internal_call("Nebula.Time::GetFrameCount", (void*)Time_GetFrameCount);
		NB_CORE_INFO("  Registered Nebula.Time functions");

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
		mono_add_internal_call("Nebula.InternalCalls::Entity_DestroyDelayed", (void*)Entity_DestroyDelayed);
		mono_add_internal_call("Nebula.InternalCalls::Entity_GetScriptInstance", (void*)Entity_GetScriptInstance);
		mono_add_internal_call("Nebula.InternalCalls::Entity_GetScriptByName", (void*)Entity_GetScriptByName);
		mono_add_internal_call("Nebula.InternalCalls::Entity_GetTag", (void*)Entity_GetTag);
		mono_add_internal_call("Nebula.InternalCalls::Entity_SetTag", (void*)Entity_SetTag);
		mono_add_internal_call("Nebula.InternalCalls::Entity_FindByTag", (void*)Entity_FindByTag);
		mono_add_internal_call("Nebula.InternalCalls::Entity_FindAllByTag", (void*)Entity_FindAllByTag);
		mono_add_internal_call("Nebula.InternalCalls::Entity_GetActiveSelf", (void*)Entity_GetActiveSelf);
		mono_add_internal_call("Nebula.InternalCalls::Entity_GetActiveInHierarchy", (void*)Entity_GetActiveInHierarchy);
		mono_add_internal_call("Nebula.InternalCalls::Entity_SetActive", (void*)Entity_SetActive);
		mono_add_internal_call("Nebula.InternalCalls::Entity_GetParent", (void*)Entity_GetParent);
		mono_add_internal_call("Nebula.InternalCalls::Entity_SetParent", (void*)Entity_SetParent);
		mono_add_internal_call("Nebula.InternalCalls::Entity_SetParentWithTransform", (void*)Entity_SetParentWithTransform);
		mono_add_internal_call("Nebula.InternalCalls::Entity_GetChildCount", (void*)Entity_GetChildCount);
		mono_add_internal_call("Nebula.InternalCalls::Entity_GetChild", (void*)Entity_GetChild);
		NB_CORE_INFO("  Registered Nebula.InternalCalls functions");

		// Register Physics functions (Nebula.Physics class)
		mono_add_internal_call("Nebula.Physics::InternalRaycast", (void*)Physics_InternalRaycast);
		mono_add_internal_call("Nebula.Physics::InternalSphereCast", (void*)Physics_InternalSphereCast);
		mono_add_internal_call("Nebula.Physics::InternalOverlapSphere", (void*)Physics_InternalOverlapSphere);
		mono_add_internal_call("Nebula.Physics::GetGravity", (void*)Physics_GetGravity);
		mono_add_internal_call("Nebula.Physics::SetGravity", (void*)Physics_SetGravity);
		NB_CORE_INFO("  Registered Nebula.Physics functions");
		
		NB_CORE_INFO("ScriptGlue: All internal calls registered successfully");

		// Component registration moved to RegisterComponents()
		// Must be called after CoreAssembly is loaded
	}

	void ScriptGlue::RegisterComponents()
	{
		RegisterComponent<TransformComponent, MeshRendererComponent, CameraComponent, TagComponent, ScriptComponent, 
			BoxColliderComponent, SphereColliderComponent, RigidBodyComponent, AudioSourceComponent>();
	}

	void ScriptGlue::Update(float deltaTime)
	{
		// Update delayed destroy entities
		Scene* scene = ScriptEngine::GetSceneContext();
		if (!scene)
			return;

		std::vector<uint32_t> toDestroy;
		for (auto& [entityID, timeRemaining] : s_DestroyDelayedEntities)
		{
			timeRemaining -= deltaTime;
			if (timeRemaining <= 0.0f)
			{
				toDestroy.push_back(entityID);
			}
		}

		for (uint32_t entityID : toDestroy)
		{
			Entity entity{ (entt::entity)entityID, scene };
			if (entity)
				scene->DestroyEntity(entity);
			s_DestroyDelayedEntities.erase(entityID);
		}
	}

	void ScriptGlue::UpdateMouseState()
	{
		// Update mouse delta
		auto [currentX, currentY] = Input::GetMousePos();
		glm::vec2 currentPos(currentX, currentY);
		s_MouseDelta = currentPos - s_LastMousePos;
		s_LastMousePos = currentPos;

		// Reset scroll delta (will be updated by events)
		s_MouseScrollDelta = glm::vec2(0.0f);
	}

}

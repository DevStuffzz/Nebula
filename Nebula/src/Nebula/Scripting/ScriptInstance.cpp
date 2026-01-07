#include "nbpch.h"
#include "ScriptInstance.h"
#include "ScriptEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>

namespace Nebula {

	extern ScriptEngineData* s_Data;

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = scriptClass->Instantiate();

		m_Constructor = s_Data->EntityClass->GetMethod(".ctor", 0);
		m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
		m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);
		m_OnDestroyMethod = scriptClass->GetMethod("OnDestroy", 0);
		m_OnCollisionEnterMethod = scriptClass->GetMethod("OnCollisionEnter", 1);
		m_OnCollisionStayMethod = scriptClass->GetMethod("OnCollisionStay", 1);
		m_OnCollisionExitMethod = scriptClass->GetMethod("OnCollisionExit", 1);
		
		MonoClass* scriptBehaviorClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Nebula", "ScriptBehavior");
		MonoProperty* entityProperty = mono_class_get_property_from_name(scriptBehaviorClass, "Entity");
		
		uint32_t entityID = (uint32_t)entity;
		
		if (entityProperty)
		{
			MonoClass* scriptEntityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Nebula", "ScriptEntity");
			MonoObject* entityInstance = mono_object_new(s_Data->AppDomain, scriptEntityClass);
			mono_runtime_object_init(entityInstance);
			
			MonoProperty* idProperty = mono_class_get_property_from_name(scriptEntityClass, "ID");
			MonoMethod* idSetter = mono_property_get_set_method(idProperty);
			void* idParams[1] = { &entityID };
			mono_runtime_invoke(idSetter, entityInstance, idParams, nullptr);
			
			MonoMethod* entitySetter = mono_property_get_set_method(entityProperty);
			void* entityParams[1] = { entityInstance };
			mono_runtime_invoke(entitySetter, m_Instance, entityParams, nullptr);
			
			NB_CORE_INFO("Entity property set successfully for entity ID {}", entityID);
		}
		else
		{
			NB_CORE_ERROR("Could not find Entity property in ScriptBehavior!");
		}
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (m_OnCreateMethod)
			m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod, nullptr);
	}

	void ScriptInstance::InvokeOnUpdate(float deltaTime)
	{
		if (m_OnUpdateMethod)
		{
			void* param = &deltaTime;
			m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
		}
	}

	void ScriptInstance::InvokeOnDestroy()
	{
		if (m_OnDestroyMethod)
			m_ScriptClass->InvokeMethod(m_Instance, m_OnDestroyMethod, nullptr);
	}

	void ScriptInstance::InvokeOnCollisionEnter(MonoObject* collision)
	{
		if (m_OnCollisionEnterMethod)
		{
			void* param = collision;
			m_ScriptClass->InvokeMethod(m_Instance, m_OnCollisionEnterMethod, &param);
		}
	}

	void ScriptInstance::InvokeOnCollisionStay(MonoObject* collision)
	{
		if (m_OnCollisionStayMethod)
		{
			void* param = collision;
			m_ScriptClass->InvokeMethod(m_Instance, m_OnCollisionStayMethod, &param);
		}
	}

	void ScriptInstance::InvokeOnCollisionExit(MonoObject* collision)
	{
		if (m_OnCollisionExitMethod)
		{
			void* param = collision;
			m_ScriptClass->InvokeMethod(m_Instance, m_OnCollisionExitMethod, &param);
		}
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_get_value(m_Instance, field.ClassField, buffer);
		return true;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_set_value(m_Instance, field.ClassField, (void*)value);
		return true;
	}

}

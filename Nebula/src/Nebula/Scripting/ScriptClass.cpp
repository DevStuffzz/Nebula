#include "nbpch.h"
#include "ScriptClass.h"
#include "ScriptEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/tabledefs.h>

namespace Nebula {

	extern ScriptEngineData* s_Data;

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());
		if (!m_MonoClass)
			m_MonoClass = mono_class_from_name(s_Data->AppAssemblyImage, classNamespace.c_str(), className.c_str());
	}

	MonoObject* ScriptClass::Instantiate()
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, m_MonoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		MonoObject* exception = nullptr;
		MonoObject* result = mono_runtime_invoke(method, instance, params, &exception);
		
		if (exception)
		{
			MonoClass* exceptionClass = mono_object_get_class(exception);
			const char* exceptionName = mono_class_get_name(exceptionClass);
			
			MonoProperty* messageProp = mono_class_get_property_from_name(exceptionClass, "Message");
			if (messageProp)
			{
				MonoMethod* messageGetter = mono_property_get_get_method(messageProp);
				MonoObject* messageObj = mono_runtime_invoke(messageGetter, exception, nullptr, nullptr);
				char* message = mono_string_to_utf8((MonoString*)messageObj);
				NB_CORE_ERROR("Mono exception in InvokeMethod: {} - {}", exceptionName, message);
				mono_free(message);
			}
			else
			{
				NB_CORE_ERROR("Mono exception in InvokeMethod: {}", exceptionName);
			}
		}
		
		return result;
	}

}

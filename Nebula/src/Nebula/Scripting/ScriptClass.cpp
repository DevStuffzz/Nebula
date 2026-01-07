#include "nbpch.h"
#include "ScriptClass.h"
#include "ScriptEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>

namespace Nebula {

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
			
			std::string message = "Unknown error";
			std::string stackTrace = "";
			
			// Get exception message
			MonoProperty* messageProp = mono_class_get_property_from_name(exceptionClass, "Message");
			if (messageProp)
			{
				MonoMethod* messageGetter = mono_property_get_get_method(messageProp);
				MonoObject* messageObj = mono_runtime_invoke(messageGetter, exception, nullptr, nullptr);
				if (messageObj)
				{
					char* msgStr = mono_string_to_utf8((MonoString*)messageObj);
					message = msgStr;
					mono_free(msgStr);
				}
			}
			
			// Get stack trace
			MonoProperty* stackTraceProp = mono_class_get_property_from_name(exceptionClass, "StackTrace");
			if (stackTraceProp)
			{
				MonoMethod* stackTraceGetter = mono_property_get_get_method(stackTraceProp);
				MonoObject* stackTraceObj = mono_runtime_invoke(stackTraceGetter, exception, nullptr, nullptr);
				if (stackTraceObj)
				{
					char* stStr = mono_string_to_utf8((MonoString*)stackTraceObj);
					stackTrace = stStr;
					mono_free(stStr);
				}
			}
			
			// Log to both engine and client (editor console)
			NB_CORE_ERROR("=== C# SCRIPT EXCEPTION ===");
			NB_CORE_ERROR("Type: {}", exceptionName);
			NB_CORE_ERROR("Message: {}", message);
			if (!stackTrace.empty())
			{
				NB_CORE_ERROR("Stack Trace:\n{}", stackTrace);
			}
			NB_CORE_ERROR("=========================");
			
			// Log to client console (editor)
			Log::LogClientMessage("[C# EXCEPTION] " + std::string(exceptionName) + ": " + message, LOG_ERROR);
			if (!stackTrace.empty())
			{
				Log::LogClientMessage(stackTrace, LOG_ERROR);
			}
			
			// Set flag to stop runtime
			s_Data->HasScriptException = true;
		}
		
		return result;
	}

}

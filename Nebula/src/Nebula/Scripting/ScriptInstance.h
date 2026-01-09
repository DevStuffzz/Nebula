#pragma once

#include "Nebula/Core.h"
#include "ScriptClass.h"
#include "Nebula/Scene/Entity.h"

namespace Nebula {

	class NEBULA_API ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

		void InvokeOnCreate();
		void InvokeOnUpdate(float deltaTime);
		void InvokeOnDestroy();
		void InvokeOnCollisionEnter(MonoObject* collision);
		void InvokeOnCollisionStay(MonoObject* collision);
		void InvokeOnCollisionExit(MonoObject* collision);
		
		Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }
		MonoObject* GetManagedObject() { return m_Instance; }

		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");

			bool success = GetFieldValueInternal(name, g_ScriptFieldValueBuffer);
			if (!success)
				return T();

			return *(T*)g_ScriptFieldValueBuffer;
		}

		template<typename T>
		void SetFieldValue(const std::string& name, T value)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			SetFieldValueInternal(name, &value);
		}

		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, const void* value);

	private:
		Ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;
		MonoMethod* m_OnDestroyMethod = nullptr;
		MonoMethod* m_OnCollisionEnterMethod = nullptr;
		MonoMethod* m_OnCollisionStayMethod = nullptr;
		MonoMethod* m_OnCollisionExitMethod = nullptr;

		friend struct ScriptFieldInstance;
	};

	// Static buffer for field value retrieval (exported for DLL)
	extern NEBULA_API char g_ScriptFieldValueBuffer[16];

}

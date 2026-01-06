#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Core.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <filesystem>

typedef struct _MonoDomain MonoDomain;
typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoImage MonoImage;
typedef struct _MonoClass MonoClass;
typedef struct _MonoObject MonoObject;
typedef struct _MonoMethod MonoMethod;
typedef struct _MonoClassField MonoClassField;
typedef struct _MonoType MonoType;

namespace Nebula {

	// Type aliases
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename... Args>
	Ref<T> CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	class Entity;
	class Scene;

	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity
	};

	struct ScriptField
	{
		ScriptFieldType Type;
		std::string Name;

		MonoClassField* ClassField;
	};

	// Helper for template use
	template<typename... Component>
	struct ComponentGroup
	{
	};

	const char* ScriptFieldTypeToString(ScriptFieldType type);
	ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType);

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className);

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		const std::unordered_map<std::string, ScriptField>& GetFields() const { return m_Fields; }
	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;

		std::unordered_map<std::string, ScriptField> m_Fields;

		MonoClass* m_MonoClass = nullptr;

		friend class ScriptEngine;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

		void InvokeOnCreate();
		void InvokeOnUpdate(float deltaTime);
		void InvokeOnDestroy();

		Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }
		MonoObject* GetManagedObject() { return m_Instance; }

		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");

			bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
			if (!success)
				return T();

			return *(T*)s_FieldValueBuffer;
		}

		template<typename T>
		void SetFieldValue(const std::string& name, T value)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");

			SetFieldValueInternal(name, &value);
		}
	private:
		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, const void* value);
	private:
		Ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;
		MonoMethod* m_OnDestroyMethod = nullptr;

		inline static char s_FieldValueBuffer[16];

		friend class ScriptEngine;
		friend class ScriptGlue;
		friend struct ScriptFieldInstance;
	};

	class NEBULA_API ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadProjectAssembly(const std::filesystem::path& assemblyPath);
		static void ReloadAssembly();

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();

		static bool EntityClassExists(const std::string& fullClassName);	static std::vector<std::string> GetEntityClassNames();
		static void OnCreateEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, float deltaTime);
		static void OnDestroyEntity(Entity entity);

		static Scene* GetSceneContext();
		static Ref<ScriptInstance> GetEntityScriptInstance(uint32_t entityID);

		static MonoImage* GetCoreAssemblyImage();
	private:
		static void InitMono();
		static void ShutdownMono();
		
		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath);
		static void LoadAssembly(const std::filesystem::path& filepath);
		static void LoadAppAssembly(const std::filesystem::path& filepath);
		
		static void LoadAssemblyClasses();

		friend class ScriptClass;
		friend class ScriptGlue;
	};

}

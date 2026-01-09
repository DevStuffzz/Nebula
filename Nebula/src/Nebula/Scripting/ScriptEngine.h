#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Core.h"
#include "ScriptClass.h"
#include "ScriptInstance.h"
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

	class Entity;
	class Scene;

	// Helper for template use
	template<typename... Component>
	struct ComponentGroup
	{
	};

	NEBULA_API const char* ScriptFieldTypeToString(ScriptFieldType type);
	NEBULA_API ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType);

	class NEBULA_API ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadProjectAssembly(const std::filesystem::path& assemblyPath);
		static void ReloadAssembly();

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();

		static bool EntityClassExists(const std::string& fullClassName);	static std::vector<std::string> GetEntityClassNames();	static Ref<ScriptClass> GetEntityScriptClass(const std::string& fullClassName);		static void OnCreateEntity(Entity entity);
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

	// Internal script engine data - for use by scripting system implementation only
	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;

		Ref<ScriptClass> EntityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		std::unordered_map<uint32_t, Ref<ScriptInstance>> EntityInstances;

		Scene* SceneContext = nullptr;
		
		// Exception tracking
		bool HasScriptException = false;
	};

	extern ScriptEngineData* s_Data;
}

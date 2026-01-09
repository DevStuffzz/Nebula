#include "nbpch.h"
#include "ScriptEngine.h"
#include "ScriptGlue.h"

#include "Nebula/Scene/Scene.h"
#include "Nebula/Scene/Entity.h"
#include "Nebula/Scene/Components.h"
#include "Nebula/Log.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/metadata.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/threads.h>

namespace Nebula {

	const char* ScriptFieldTypeToString(ScriptFieldType type)
	{
		switch (type)
		{
			case ScriptFieldType::None:    return "None";
			case ScriptFieldType::Float:   return "Float";
			case ScriptFieldType::Double:  return "Double";
			case ScriptFieldType::Bool:    return "Bool";
			case ScriptFieldType::Char:    return "Char";
			case ScriptFieldType::Byte:    return "Byte";
			case ScriptFieldType::Short:   return "Short";
			case ScriptFieldType::Int:     return "Int";
			case ScriptFieldType::Long:    return "Long";
			case ScriptFieldType::UByte:   return "UByte";
			case ScriptFieldType::UShort:  return "UShort";
			case ScriptFieldType::UInt:    return "UInt";
			case ScriptFieldType::ULong:   return "ULong";
			case ScriptFieldType::Vector2: return "Vector2";
			case ScriptFieldType::Vector3: return "Vector3";
			case ScriptFieldType::Vector4: return "Vector4";
			case ScriptFieldType::Entity:  return "Entity";
		}
		return "None";
	}

	ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
	{
		int type = mono_type_get_type(monoType);

		switch (type)
		{
			case MONO_TYPE_R4:      return ScriptFieldType::Float;
			case MONO_TYPE_R8:      return ScriptFieldType::Double;
			case MONO_TYPE_BOOLEAN: return ScriptFieldType::Bool;
			case MONO_TYPE_CHAR:    return ScriptFieldType::Char;
			case MONO_TYPE_I1:      return ScriptFieldType::Byte;
			case MONO_TYPE_I2:      return ScriptFieldType::Short;
			case MONO_TYPE_I4:      return ScriptFieldType::Int;
			case MONO_TYPE_I8:      return ScriptFieldType::Long;
			case MONO_TYPE_U1:      return ScriptFieldType::UByte;
			case MONO_TYPE_U2:      return ScriptFieldType::UShort;
			case MONO_TYPE_U4:      return ScriptFieldType::UInt;
			case MONO_TYPE_U8:      return ScriptFieldType::ULong;
		}
		return ScriptFieldType::None;
	}

	// Definition of the script engine data pointer
	ScriptEngineData* s_Data = nullptr;

	static void PrintAssemblyTypes(MonoAssembly* assembly)
{
	MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			NB_CORE_TRACE("{0}.{1}", nameSpace, name);
		}
	}

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();

		InitMono();
		ScriptGlue::RegisterFunctions();

		// Don't load assemblies here - they will be loaded when a project is opened
		// NebulaScriptCore and project scripts are loaded in LoadProjectAssembly()
	}

void ScriptEngine::Shutdown()
{
	s_Data->EntityClasses.clear();

	mono_domain_set(mono_get_root_domain(), false);
	if (s_Data->AppDomain)
		mono_domain_unload(s_Data->AppDomain);

	s_Data->AppDomain = nullptr;
	s_Data->RootDomain = nullptr;

	delete s_Data;
	s_Data = nullptr;
}

void ScriptEngine::OnRuntimeStart(Scene* scene)
{
	s_Data->SceneContext = scene;
	s_Data->HasScriptException = false;  // Reset exception flag
}

void ScriptEngine::OnRuntimeStop()
{
	s_Data->SceneContext = nullptr;
	s_Data->EntityInstances.clear();
}

	bool ScriptEngine::EntityClassExists(const std::string& fullClassName)
	{
		return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
	}

	std::vector<std::string> ScriptEngine::GetEntityClassNames()
	{
		std::vector<std::string> classNames;
		for (const auto& [name, scriptClass] : s_Data->EntityClasses)
		{
			classNames.push_back(name);
		}
		return classNames;
	}

	Ref<ScriptClass> ScriptEngine::GetEntityScriptClass(const std::string& fullClassName)
	{
		if (s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end())
			return s_Data->EntityClasses[fullClassName];
		return nullptr;
	}

	void ScriptEngine::OnCreateEntity(Entity entity)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();
		NB_CORE_INFO("OnCreateEntity called for class: {}", sc.ClassName);
		
		if (ScriptEngine::EntityClassExists(sc.ClassName))
		{
			uint32_t entityID = (uint32_t)entity;
			
			Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);
			s_Data->EntityInstances[entityID] = instance;

			// Apply stored field values from component to instance
			for (const auto& [fieldName, fieldValue] : sc.FieldValues)
			{
				switch (fieldValue.VarType)
				{
				case ScriptVariable::Type::Float:
					instance->SetFieldValue(fieldName, fieldValue.FloatValue);
					break;
				case ScriptVariable::Type::Int:
					instance->SetFieldValue(fieldName, fieldValue.IntValue);
					break;
				case ScriptVariable::Type::Bool:
					instance->SetFieldValue(fieldName, fieldValue.BoolValue);
					break;
				default:
					break;
				}
			}

			// Call OnCreate
			NB_CORE_INFO("Calling OnCreate for entity {}", entityID);
			instance->InvokeOnCreate();
		}
		else
		{
			NB_CORE_ERROR("Script class '{}' not found in loaded assemblies!", sc.ClassName);
		}
	}

	void ScriptEngine::OnUpdateEntity(Entity entity, float deltaTime)
	{
		// Check if script exception occurred - stop runtime if so
		if (s_Data->HasScriptException)
		{
			if (s_Data->SceneContext)
			{
				NB_CORE_WARN("Stopping runtime due to C# script exception");
				Log::LogClientMessage("Runtime stopped due to C# script exception", LOG_WARN);
				s_Data->SceneContext->OnRuntimeStop();
				s_Data->HasScriptException = false;  // Reset flag
			}
			return;
		}
		
		uint32_t entityID = (uint32_t)entity;
		if (s_Data->EntityInstances.find(entityID) != s_Data->EntityInstances.end())
		{
			Ref<ScriptInstance> instance = s_Data->EntityInstances[entityID];
			instance->InvokeOnUpdate(deltaTime);
		}
	}

	void ScriptEngine::OnDestroyEntity(Entity entity)
	{
		uint32_t entityID = (uint32_t)entity;
		if (s_Data->EntityInstances.find(entityID) != s_Data->EntityInstances.end())
		{
			Ref<ScriptInstance> instance = s_Data->EntityInstances[entityID];
			instance->InvokeOnDestroy();
			s_Data->EntityInstances.erase(entityID);
		}
	}

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_Data->SceneContext;
	}

	Ref<ScriptInstance> ScriptEngine::GetEntityScriptInstance(uint32_t entityID)
	{
		auto it = s_Data->EntityInstances.find(entityID);
		if (it == s_Data->EntityInstances.end())
			return nullptr;

		return it->second;
	}

	void ScriptEngine::InitMono()
	{
		// Get the directory where the executable is located
		char exePath[MAX_PATH];
#ifdef _WIN32
		GetModuleFileNameA(nullptr, exePath, MAX_PATH);
#endif
		std::filesystem::path exeDir = std::filesystem::path(exePath).parent_path();
		std::filesystem::path monoLibPath = exeDir / "lib";
		std::filesystem::path monoEtcPath = exeDir / "etc";

		// Set Mono assembly and config directories using absolute paths
		mono_set_dirs(monoLibPath.string().c_str(), monoEtcPath.string().c_str());

		MonoDomain* rootDomain = mono_jit_init("NebulaJITRuntime");
	if (!rootDomain)
	{
		NB_CORE_ERROR("Failed to initialize Mono JIT!");
		return;
	}
		s_Data->RootDomain = rootDomain;
	}

	void ScriptEngine::ShutdownMono()
	{
		// mono_jit_cleanup(s_Data->RootDomain);
	}

	MonoAssembly* ScriptEngine::LoadMonoAssembly(const std::filesystem::path& assemblyPath)
	{
		std::ifstream stream(assemblyPath, std::ios::binary | std::ios::ate);
	if (!stream.is_open())
	{
		NB_CORE_ERROR("Failed to open assembly file: {}", assemblyPath.string());
		return nullptr;
	}

		std::streampos size = stream.tellg();
		stream.seekg(0, std::ios::beg);

		char* data = new char[size];
		stream.read(data, size);
		stream.close();

		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(data, size, 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			const char* errorMessage = mono_image_strerror(status);
			NB_CORE_ERROR("Failed to load Mono image: {0}", errorMessage);
			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.string().c_str(), &status, 0);
		delete[] data;

		return assembly;
	}

	void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		// Create an App Domain
		s_Data->AppDomain = mono_domain_create_appdomain((char*)"NebulaScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssembly = LoadMonoAssembly(filepath);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	}

	void ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
	{
		s_Data->AppAssembly = LoadMonoAssembly(filepath);
		if (s_Data->AppAssembly)
		{
			s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
			LoadAssemblyClasses();
		}
	}

	void ScriptEngine::LoadProjectAssembly(const std::filesystem::path& assemblyPath)
	{
		if (!std::filesystem::exists(assemblyPath))
		{
			NB_CORE_ERROR("Project assembly not found: {0}", assemblyPath.string());
			return;
		}

		// Unload previous app domain if exists
		if (s_Data->AppDomain)
		{
			s_Data->EntityInstances.clear();
			mono_domain_set(s_Data->RootDomain, false);
			mono_domain_unload(s_Data->AppDomain);
			s_Data->AppDomain = nullptr;
		}

		// Create new app domain
		s_Data->AppDomain = mono_domain_create_appdomain((char*)"NebulaScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		// Reload core assembly from project's Library folder
		std::filesystem::path corePath = "Library/NebulaScriptCore.dll";
		s_Data->CoreAssembly = LoadMonoAssembly(corePath);
		if (!s_Data->CoreAssembly)
		{
			NB_CORE_ERROR("Failed to load NebulaScriptCore assembly!");
			return;
		}
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);

		// Initialize EntityClass from NebulaScriptCore
		s_Data->EntityClass = CreateRef<ScriptClass>("Nebula", "ScriptEntity");

		// Register components now that CoreAssembly is loaded
		ScriptGlue::RegisterComponents();

		// Load project assembly
		LoadAppAssembly(assemblyPath);
		
		NB_CORE_INFO("Loaded project assembly: {0}", assemblyPath.string());
	}

	void ScriptEngine::ReloadAssembly()
	{
		// TODO: Implement hot reload
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_Data->EntityClasses.clear();

		NB_CORE_INFO("ScriptEngine: Loading assembly classes...");
		
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		NB_CORE_INFO("  Loading types from assembly");
		
		MonoClass* scriptBehaviorClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Nebula", "ScriptBehavior");
		if (!scriptBehaviorClass)
		{
			NB_CORE_ERROR("  Failed to find ScriptBehavior base class!");
			return;
		}
		NB_CORE_INFO("  Found ScriptBehavior base class");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, className);
			else
				fullName = className;

			MonoClass* monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, className);

			if (monoClass == scriptBehaviorClass)
				continue;

			bool isScriptBehavior = mono_class_is_subclass_of(monoClass, scriptBehaviorClass, false);
			if (!isScriptBehavior)
				continue;

			Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, className);
			s_Data->EntityClasses[fullName] = scriptClass;
			NB_CORE_INFO("  Registered script class");

			// This routine is an iterator routine for retrieving the fields in a class.
			// You must pass a gpointer that points to zero and is treated as an opaque handle
			// to iterate over all of the elements. When no more values are available, the return value is NULL.

			int fieldCount = mono_class_num_fields(monoClass);
			NB_CORE_WARN("{} has {} fields:", className, fieldCount);
			void* iterator = nullptr;
			while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
			{
				const char* fieldName = mono_field_get_name(field);
				uint32_t flags = mono_field_get_flags(field);
				
				// Check if field is public or has [SerializeField] attribute
				bool isPublic = flags & MONO_FIELD_ATTR_PUBLIC;
				bool hasSerializeField = false;
				bool hasHideInInspector = false;
				
				// Check for attributes
				MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_field(monoClass, field);
				if (attrInfo)
				{
					MonoArray* attrs = mono_custom_attrs_construct(attrInfo);
					if (attrs)
					{
						int attrCount = mono_array_length(attrs);
						for (int i = 0; i < attrCount; i++)
						{
							MonoObject* attr = mono_array_get(attrs, MonoObject*, i);
							MonoClass* attrClass = mono_object_get_class(attr);
							const char* attrName = mono_class_get_name(attrClass);
							
							if (strcmp(attrName, "SerializeFieldAttribute") == 0)
								hasSerializeField = true;
							if (strcmp(attrName, "HideInInspectorAttribute") == 0)
								hasHideInInspector = true;
						}
					}
				}
				
				// Include field if it's public (and not hidden) or has SerializeField
				bool shouldSerialize = (isPublic && !hasHideInInspector) || hasSerializeField;
				
				if (shouldSerialize)
				{
					MonoType* type = mono_field_get_type(field);
					ScriptFieldType fieldType = MonoTypeToScriptFieldType(type);
					NB_CORE_WARN("  {} ({}) {}", fieldName, ScriptFieldTypeToString(fieldType), 
						hasSerializeField ? "[SerializeField]" : "");

					scriptClass->m_Fields[fieldName] = { fieldType, fieldName, field };
				}
			}
		}

		/*auto& entityClasses = s_Data->EntityClasses;

		// We need to store references to these on the C++ side
		mono_field_static_set_value(mono_class_vtable(s_Data->AppDomain, monoClass), field, &value);*/
	}

	MonoImage* ScriptEngine::GetCoreAssemblyImage()
	{
		return s_Data->CoreAssemblyImage;
	}

}

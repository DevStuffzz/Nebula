#pragma once

#include "Nebula/Core.h"
#include <string>
#include <memory>
#include <unordered_map>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace Nebula {

	class Entity;
	class Scene;

	class NEBULA_API LuaScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		// Script execution
		static bool LoadScript(const std::string& filepath);
		static bool ExecuteString(const std::string& code);
		
		// Entity script management
		static void OnCreateEntity(Entity entity, const std::string& scriptPath);
		static void OnUpdateEntity(Entity entity, float deltaTime);
		static void OnDestroyEntity(Entity entity);

		// Script reloading
		static bool ReloadScript(const std::string& filepath);
		
		// Lua state access
		static lua_State* GetState() { return s_LuaState; }

		// Error handling
		static std::string GetLastError() { return s_LastError; }

	private:
		static void RegisterEngineFunctions();
		static void SetupLuaPath();
		static bool CheckLua(int result);

	private:
		static lua_State* s_LuaState;
		static std::string s_LastError;
		static std::unordered_map<uint32_t, std::string> s_EntityScripts; // EntityID -> ScriptPath
	};

}

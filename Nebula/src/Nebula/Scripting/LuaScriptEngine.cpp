#include "nbpch.h"
#include "LuaScriptEngine.h"
#include "LuaBindings.h"
#include "Nebula/Log.h"
#include "Nebula/Scene/Entity.h"
#include <fstream>
#include <sstream>

namespace Nebula {

	lua_State* LuaScriptEngine::s_LuaState = nullptr;
	std::string LuaScriptEngine::s_LastError = "";
	std::unordered_map<uint32_t, std::string> LuaScriptEngine::s_EntityScripts;

	void LuaScriptEngine::Init()
	{
		NB_CORE_INFO("Initializing Lua Script Engine...");
		
		// Create new Lua state
		s_LuaState = luaL_newstate();
		if (!s_LuaState)
		{
			NB_CORE_ERROR("Failed to create Lua state!");
			return;
		}

		// Load standard Lua libraries
		luaL_openlibs(s_LuaState);

		// Setup Lua path for require statements
		SetupLuaPath();

		// Register engine API bindings
		RegisterEngineFunctions();

		NB_CORE_INFO("Lua Script Engine initialized successfully");
	}

	void LuaScriptEngine::Shutdown()
	{
		if (s_LuaState)
		{
			lua_close(s_LuaState);
			s_LuaState = nullptr;
		}
		s_EntityScripts.clear();
		NB_CORE_INFO("Lua Script Engine shut down");
	}

	bool LuaScriptEngine::LoadScript(const std::string& filepath)
	{
		if (!s_LuaState)
		{
			NB_CORE_ERROR("Lua state not initialized!");
			return false;
		}

		std::ifstream file(filepath);
		if (!file.is_open())
		{
			s_LastError = "Failed to open file: " + filepath;
			NB_CORE_ERROR("{0}", s_LastError);
			return false;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string code = buffer.str();
		file.close();

		// Load the script
		int result = luaL_loadstring(s_LuaState, code.c_str());
		if (!CheckLua(result))
			return false;

		// Execute the script
		result = lua_pcall(s_LuaState, 0, 0, 0);
		if (!CheckLua(result))
			return false;

		NB_CORE_INFO("Loaded Lua script: {0}", filepath);
		return true;
	}

	bool LuaScriptEngine::ExecuteString(const std::string& code)
	{
		if (!s_LuaState)
		{
			NB_CORE_ERROR("Lua state not initialized!");
			return false;
		}

		int result = luaL_dostring(s_LuaState, code.c_str());
		return CheckLua(result);
	}

	void LuaScriptEngine::OnCreateEntity(Entity entity, const std::string& scriptPath)
	{
		if (!s_LuaState)
			return;

		// Store the script path for this entity
		s_EntityScripts[(uint32_t)entity] = scriptPath;

		// Load the script if not already loaded
		LoadScript(scriptPath);

		// Call OnCreate function if it exists
		lua_getglobal(s_LuaState, "OnCreate");
		if (lua_isfunction(s_LuaState, -1))
		{
			// Create userdata for entity
			Entity* entityPtr = (Entity*)lua_newuserdata(s_LuaState, sizeof(Entity));
			*entityPtr = entity;
			luaL_getmetatable(s_LuaState, "EntityMetatable");
			lua_setmetatable(s_LuaState, -2);

			if (lua_pcall(s_LuaState, 1, 0, 0) != LUA_OK)
			{
				CheckLua(LUA_ERRRUN);
				lua_pop(s_LuaState, 1);
			}
		}
		else
		{
			lua_pop(s_LuaState, 1);
		}
	}

	void LuaScriptEngine::OnUpdateEntity(Entity entity, float deltaTime)
	{
		if (!s_LuaState)
			return;

		// Call OnUpdate function if it exists
		lua_getglobal(s_LuaState, "OnUpdate");
		if (lua_isfunction(s_LuaState, -1))
		{
			// Create userdata for entity
			Entity* entityPtr = (Entity*)lua_newuserdata(s_LuaState, sizeof(Entity));
			*entityPtr = entity;
			luaL_getmetatable(s_LuaState, "EntityMetatable");
			lua_setmetatable(s_LuaState, -2);

			lua_pushnumber(s_LuaState, deltaTime);
			if (lua_pcall(s_LuaState, 2, 0, 0) != LUA_OK)
			{
				CheckLua(LUA_ERRRUN);
				lua_pop(s_LuaState, 1);
			}
		}
		else
		{
			lua_pop(s_LuaState, 1);
		}
	}

	void LuaScriptEngine::OnDestroyEntity(Entity entity)
	{
		if (!s_LuaState)
			return;

		// Call OnDestroy function if it exists
		lua_getglobal(s_LuaState, "OnDestroy");
		if (lua_isfunction(s_LuaState, -1))
		{
			lua_pushlightuserdata(s_LuaState, (void*)&entity);
			if (lua_pcall(s_LuaState, 1, 0, 0) != LUA_OK)
			{
				CheckLua(LUA_ERRRUN);
				lua_pop(s_LuaState, 1);
			}
		}
		else
		{
			lua_pop(s_LuaState, 1);
		}

		// Remove from entity scripts map
		s_EntityScripts.erase((uint32_t)entity);
	}

	bool LuaScriptEngine::ReloadScript(const std::string& filepath)
	{
		NB_CORE_INFO("Reloading Lua script: {0}", filepath);
		return LoadScript(filepath);
	}

	void LuaScriptEngine::RegisterEngineFunctions()
	{
		// Register all engine API bindings
		LuaBindings::RegisterAll(s_LuaState);

		// Create Entity metatable for entity methods
		luaL_newmetatable(s_LuaState, "EntityMetatable");

		// Set __index to self
		lua_pushvalue(s_LuaState, -1);
		lua_setfield(s_LuaState, -2, "__index");

		// Register entity methods
		lua_pushcfunction(s_LuaState, LuaBindings::Entity_GetPosition);
		lua_setfield(s_LuaState, -2, "GetPosition");

		lua_pushcfunction(s_LuaState, LuaBindings::Entity_SetPosition);
		lua_setfield(s_LuaState, -2, "SetPosition");

		lua_pushcfunction(s_LuaState, LuaBindings::Entity_GetRotation);
		lua_setfield(s_LuaState, -2, "GetRotation");

		lua_pushcfunction(s_LuaState, LuaBindings::Entity_SetRotation);
		lua_setfield(s_LuaState, -2, "SetRotation");

		lua_pushcfunction(s_LuaState, LuaBindings::Entity_GetScale);
		lua_setfield(s_LuaState, -2, "GetScale");

		lua_pushcfunction(s_LuaState, LuaBindings::Entity_SetScale);
		lua_setfield(s_LuaState, -2, "SetScale");

		lua_pop(s_LuaState, 1); // Pop metatable
	}

	void LuaScriptEngine::SetupLuaPath()
	{
		// Add assets/scripts to the Lua search path
		const char* luaPath = 
			"package.path = package.path .. ';./assets/scripts/?.lua;./assets/scripts/?/init.lua'";
		luaL_dostring(s_LuaState, luaPath);
	}

	bool LuaScriptEngine::CheckLua(int result)
	{
		if (result != LUA_OK)
		{
			s_LastError = lua_tostring(s_LuaState, -1);
			NB_CORE_ERROR("Lua Error: {0}", s_LastError);
			lua_pop(s_LuaState, 1);
			return false;
		}
		return true;
	}

}

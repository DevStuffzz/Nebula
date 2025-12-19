#include "nbpch.h"
#include "LuaScriptEngine.h"
#include "LuaBindings.h"
#include "Nebula/Log.h"
#include "Nebula/Scene/Entity.h"
#include "Nebula/Scene/Components.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <regex>

namespace Nebula {

	lua_State* LuaScriptEngine::s_LuaState = nullptr;
	std::string LuaScriptEngine::s_LastError = "";
	std::unordered_map<uint32_t, std::string> LuaScriptEngine::s_EntityScripts;
	std::unordered_map<std::string, long long> LuaScriptEngine::s_ScriptModificationTimes;

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
		namespace fs = std::filesystem;

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

		// Track modification time for hot-reloading
		if (fs::exists(filepath))
		{
			auto writeTime = fs::last_write_time(filepath);
			s_ScriptModificationTimes[filepath] = writeTime.time_since_epoch().count();
		}

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

		// Physics/RigidBody methods
		lua_pushcfunction(s_LuaState, LuaBindings::Entity_AddForce);
		lua_setfield(s_LuaState, -2, "AddForce");

		lua_pushcfunction(s_LuaState, LuaBindings::Entity_SetVelocity);
		lua_setfield(s_LuaState, -2, "SetVelocity");

		lua_pushcfunction(s_LuaState, LuaBindings::Entity_GetVelocity);
		lua_setfield(s_LuaState, -2, "GetVelocity");

		lua_pushcfunction(s_LuaState, LuaBindings::Entity_SetAngularVelocity);
		lua_setfield(s_LuaState, -2, "SetAngularVelocity");

		lua_pushcfunction(s_LuaState, LuaBindings::Entity_GetAngularVelocity);
		lua_setfield(s_LuaState, -2, "GetAngularVelocity");

		lua_pushcfunction(s_LuaState, LuaBindings::Entity_SetMass);
		lua_setfield(s_LuaState, -2, "SetMass");

		lua_pushcfunction(s_LuaState, LuaBindings::Entity_GetMass);
		lua_setfield(s_LuaState, -2, "GetMass");

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

	bool LuaScriptEngine::ValidateScript(const std::string& filepath)
	{
		if (!s_LuaState)
		{
			s_LastError = "Lua state not initialized!";
			NB_CORE_ERROR("{0}", s_LastError);
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

		// Try to load the script (syntax check only)
		int result = luaL_loadstring(s_LuaState, code.c_str());
		if (result != LUA_OK)
		{
			s_LastError = lua_tostring(s_LuaState, -1);
			NB_CORE_ERROR("Lua validation error in {0}: {1}", filepath, s_LastError);
			lua_pop(s_LuaState, 1);
			return false;
		}

		// Pop the loaded chunk without executing it
		lua_pop(s_LuaState, 1);
		s_LastError.clear();
		return true;
	}

	void LuaScriptEngine::CheckForScriptChanges()
	{
		namespace fs = std::filesystem;

		// Check each tracked script for modifications
		for (auto& [scriptPath, lastModTime] : s_ScriptModificationTimes)
		{
			if (fs::exists(scriptPath))
			{
				auto writeTime = fs::last_write_time(scriptPath);
				long long currentTime = writeTime.time_since_epoch().count();

				// If file has been modified
				if (currentTime != lastModTime)
				{
					NB_CORE_INFO("Script modified, reloading: {0}", scriptPath);
					
					// Validate script before reloading
					if (ValidateScript(scriptPath))
					{
						ReloadScript(scriptPath);
						lastModTime = currentTime;
					}
					else
					{
						NB_CORE_ERROR("Script reload failed, keeping old version: {0}", scriptPath);
					}
				}
			}
		}
	}

	std::vector<ScriptVariable> LuaScriptEngine::ParseScriptVariables(const std::string& filepath)
	{
		std::vector<ScriptVariable> variables;

		std::ifstream file(filepath);
		if (!file.is_open())
		{
			NB_CORE_ERROR("Failed to open script for parsing: {0}", filepath);
			return variables;
		}

		std::string line;
		// Match both "local var = value" and "var = var or value" patterns with -- @editor
		std::regex editorVarRegex(R"((?:local\s+)?(\w+)\s*=\s*(?:\w+\s+or\s+)?([^\s]+).*--\s*@editor)");
		
		while (std::getline(file, line))
		{
			std::smatch match;
			if (std::regex_search(line, match, editorVarRegex))
			{
				std::string varName = match[1].str();
				std::string valueStr = match[2].str();

				ScriptVariable var;
				var.Name = varName;

				// Determine type from value
				if (valueStr == "true" || valueStr == "false")
				{
					// Boolean
					var.VarType = ScriptVariable::Type::Bool;
					var.BoolValue = (valueStr == "true");
				}
				else if (valueStr.find('.') != std::string::npos)
				{
					// Float (has decimal point)
					var.VarType = ScriptVariable::Type::Float;
					var.FloatValue = std::stof(valueStr);
				}
				else if (std::isdigit(valueStr[0]) || valueStr[0] == '-')
				{
					// Int (no decimal point)
					var.VarType = ScriptVariable::Type::Int;
					var.IntValue = std::stoi(valueStr);
				}
				else if (valueStr[0] == '"' || valueStr[0] == '\'')
				{
					// String
					var.VarType = ScriptVariable::Type::String;
					var.StringValue = valueStr.substr(1, valueStr.length() - 2);
				}
				else
				{
					// Default to float
					var.VarType = ScriptVariable::Type::Float;
					var.FloatValue = 0.0f;
				}

				variables.push_back(var);
			}
		}

		file.close();
		return variables;
	}

	void LuaScriptEngine::SetScriptVariables(const std::vector<ScriptVariable>& variables)
	{
		if (!s_LuaState)
			return;

		for (const auto& var : variables)
		{
			switch (var.VarType)
			{
			case ScriptVariable::Type::Float:
				lua_pushnumber(s_LuaState, var.FloatValue);
				lua_setglobal(s_LuaState, var.Name.c_str());
				break;
			case ScriptVariable::Type::Int:
				lua_pushinteger(s_LuaState, var.IntValue);
				lua_setglobal(s_LuaState, var.Name.c_str());
				break;
			case ScriptVariable::Type::Bool:
				lua_pushboolean(s_LuaState, var.BoolValue);
				lua_setglobal(s_LuaState, var.Name.c_str());
				break;
			case ScriptVariable::Type::String:
				lua_pushstring(s_LuaState, var.StringValue.c_str());
				lua_setglobal(s_LuaState, var.Name.c_str());
				break;
			case ScriptVariable::Type::Vec3:
				// Create a table for vec3
				lua_newtable(s_LuaState);
				lua_pushnumber(s_LuaState, var.Vec3Value.x);
				lua_setfield(s_LuaState, -2, "x");
				lua_pushnumber(s_LuaState, var.Vec3Value.y);
				lua_setfield(s_LuaState, -2, "y");
				lua_pushnumber(s_LuaState, var.Vec3Value.z);
				lua_setfield(s_LuaState, -2, "z");
				lua_setglobal(s_LuaState, var.Name.c_str());
				break;
			default:
				break;
			}
		}
	}

}

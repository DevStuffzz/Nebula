#pragma once
#pragma warning(disable: 4251)

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace Nebula {

	class LuaBindings
	{
	public:
		static void RegisterAll(lua_State* L);

		// Transform component functions
		static int Entity_GetPosition(lua_State* L);
		static int Entity_SetPosition(lua_State* L);
		static int Entity_GetRotation(lua_State* L);
		static int Entity_SetRotation(lua_State* L);
		static int Entity_GetScale(lua_State* L);
		static int Entity_SetScale(lua_State* L);

		// Logging functions
		static int LogInfo(lua_State* L);
		static int LogWarn(lua_State* L);
		static int LogError(lua_State* L);

	// Scene management functions
	static int LoadScene(lua_State* L);

	// Physics/RigidBody functions
	static int Entity_AddForce(lua_State* L);
	static int Entity_SetVelocity(lua_State* L);
	static int Entity_GetVelocity(lua_State* L);
	static int Entity_SetAngularVelocity(lua_State* L);
	static int Entity_GetAngularVelocity(lua_State* L);
	static int Entity_SetMass(lua_State* L);
	static int Entity_GetMass(lua_State* L);

private:
	static void RegisterInputBindings(lua_State* L);
	static void RegisterMathBindings(lua_State* L);
	static void RegisterLogBindings(lua_State* L);

	// Input functions
	static int IsKeyPressed(lua_State* L);
	static int IsMouseButtonPressed(lua_State* L);
	static int GetMousePosition(lua_State* L);
};

}

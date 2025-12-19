#include "nbpch.h"
#include "LuaBindings.h"
#include "Nebula/Scene/Scene.h"
#include "Nebula/Scene/Entity.h"
#include "Nebula/Scene/Components.h"
#include "Nebula/Input.h"
#include "Nebula/Log.h"
#include "Nebula/Keycodes.h"
#include "Nebula/MouseButtonCodes.h"
#include <btBulletDynamicsCommon.h>

namespace Nebula {

	void LuaBindings::RegisterAll(lua_State* L)
	{
		// Create the Nebula namespace table
		lua_newtable(L);

		RegisterInputBindings(L);
		RegisterLogBindings(L);

		lua_setglobal(L, "Nebula");
	}

	void LuaBindings::RegisterTransformComponent(lua_State* L)
	{
		// Entity metatable is set up in LuaScriptEngine when we create entity userdata
	}

	void LuaBindings::RegisterInputBindings(lua_State* L)
	{
		// Register IsKeyPressed function
		lua_pushcfunction(L, [](lua_State* L) -> int {
			int keycode = (int)lua_tointeger(L, 1);
			bool pressed = Input::IsKeyPressed(keycode);
			lua_pushboolean(L, pressed);
			return 1;
		});
		lua_setfield(L, -2, "IsKeyPressed");

		// Register IsMouseButtonPressed function
		lua_pushcfunction(L, [](lua_State* L) -> int {
			int button = (int)lua_tointeger(L, 1);
			bool pressed = Input::IsMouseButtonPressed(button);
			lua_pushboolean(L, pressed);
			return 1;
		});
		lua_setfield(L, -2, "IsMouseButtonPressed");

		// Register GetMousePosition function
		lua_pushcfunction(L, [](lua_State* L) -> int {
			auto pos = Input::GetMousePos();
			lua_newtable(L);
			lua_pushnumber(L, pos.first); lua_setfield(L, -2, "x");
			lua_pushnumber(L, pos.second); lua_setfield(L, -2, "y");
			return 1;
		});
		lua_setfield(L, -2, "GetMousePosition");

		// Register GetMouseX function
		lua_pushcfunction(L, [](lua_State* L) -> int {
			lua_pushnumber(L, Input::GetMouseX());
			return 1;
		});
		lua_setfield(L, -2, "GetMouseX");

		// Register GetMouseY function
		lua_pushcfunction(L, [](lua_State* L) -> int {
			lua_pushnumber(L, Input::GetMouseY());
			return 1;
		});
		lua_setfield(L, -2, "GetMouseY");

		// Register all key constants
		lua_pushinteger(L, NB_KEY_SPACE); lua_setfield(L, -2, "Space");
		lua_pushinteger(L, NB_KEY_APOSTROPHE); lua_setfield(L, -2, "Apostrophe");
		lua_pushinteger(L, NB_KEY_COMMA); lua_setfield(L, -2, "Comma");
		lua_pushinteger(L, NB_KEY_MINUS); lua_setfield(L, -2, "Minus");
		lua_pushinteger(L, NB_KEY_PERIOD); lua_setfield(L, -2, "Period");
		lua_pushinteger(L, NB_KEY_SLASH); lua_setfield(L, -2, "Slash");
		lua_pushinteger(L, NB_KEY_0); lua_setfield(L, -2, "Key0");
		lua_pushinteger(L, NB_KEY_1); lua_setfield(L, -2, "Key1");
		lua_pushinteger(L, NB_KEY_2); lua_setfield(L, -2, "Key2");
		lua_pushinteger(L, NB_KEY_3); lua_setfield(L, -2, "Key3");
		lua_pushinteger(L, NB_KEY_4); lua_setfield(L, -2, "Key4");
		lua_pushinteger(L, NB_KEY_5); lua_setfield(L, -2, "Key5");
		lua_pushinteger(L, NB_KEY_6); lua_setfield(L, -2, "Key6");
		lua_pushinteger(L, NB_KEY_7); lua_setfield(L, -2, "Key7");
		lua_pushinteger(L, NB_KEY_8); lua_setfield(L, -2, "Key8");
		lua_pushinteger(L, NB_KEY_9); lua_setfield(L, -2, "Key9");
		lua_pushinteger(L, NB_KEY_SEMICOLON); lua_setfield(L, -2, "Semicolon");
		lua_pushinteger(L, NB_KEY_EQUAL); lua_setfield(L, -2, "Equal");
		lua_pushinteger(L, NB_KEY_A); lua_setfield(L, -2, "A");
		lua_pushinteger(L, NB_KEY_B); lua_setfield(L, -2, "B");
		lua_pushinteger(L, NB_KEY_C); lua_setfield(L, -2, "C");
		lua_pushinteger(L, NB_KEY_D); lua_setfield(L, -2, "D");
		lua_pushinteger(L, NB_KEY_E); lua_setfield(L, -2, "E");
		lua_pushinteger(L, NB_KEY_F); lua_setfield(L, -2, "F");
		lua_pushinteger(L, NB_KEY_G); lua_setfield(L, -2, "G");
		lua_pushinteger(L, NB_KEY_H); lua_setfield(L, -2, "H");
		lua_pushinteger(L, NB_KEY_I); lua_setfield(L, -2, "I");
		lua_pushinteger(L, NB_KEY_J); lua_setfield(L, -2, "J");
		lua_pushinteger(L, NB_KEY_K); lua_setfield(L, -2, "K");
		lua_pushinteger(L, NB_KEY_L); lua_setfield(L, -2, "L");
		lua_pushinteger(L, NB_KEY_M); lua_setfield(L, -2, "M");
		lua_pushinteger(L, NB_KEY_N); lua_setfield(L, -2, "N");
		lua_pushinteger(L, NB_KEY_O); lua_setfield(L, -2, "O");
		lua_pushinteger(L, NB_KEY_P); lua_setfield(L, -2, "P");
		lua_pushinteger(L, NB_KEY_Q); lua_setfield(L, -2, "Q");
		lua_pushinteger(L, NB_KEY_R); lua_setfield(L, -2, "R");
		lua_pushinteger(L, NB_KEY_S); lua_setfield(L, -2, "S");
		lua_pushinteger(L, NB_KEY_T); lua_setfield(L, -2, "T");
		lua_pushinteger(L, NB_KEY_U); lua_setfield(L, -2, "U");
		lua_pushinteger(L, NB_KEY_V); lua_setfield(L, -2, "V");
		lua_pushinteger(L, NB_KEY_W); lua_setfield(L, -2, "W");
		lua_pushinteger(L, NB_KEY_X); lua_setfield(L, -2, "X");
		lua_pushinteger(L, NB_KEY_Y); lua_setfield(L, -2, "Y");
		lua_pushinteger(L, NB_KEY_Z); lua_setfield(L, -2, "Z");
		lua_pushinteger(L, NB_KEY_LEFT_BRACKET); lua_setfield(L, -2, "LeftBracket");
		lua_pushinteger(L, NB_KEY_BACKSLASH); lua_setfield(L, -2, "Backslash");
		lua_pushinteger(L, NB_KEY_RIGHT_BRACKET); lua_setfield(L, -2, "RightBracket");
		lua_pushinteger(L, NB_KEY_GRAVE_ACCENT); lua_setfield(L, -2, "GraveAccent");
		lua_pushinteger(L, NB_KEY_ESCAPE); lua_setfield(L, -2, "Escape");
		lua_pushinteger(L, NB_KEY_ENTER); lua_setfield(L, -2, "Enter");
		lua_pushinteger(L, NB_KEY_TAB); lua_setfield(L, -2, "Tab");
		lua_pushinteger(L, NB_KEY_BACKSPACE); lua_setfield(L, -2, "Backspace");
		lua_pushinteger(L, NB_KEY_INSERT); lua_setfield(L, -2, "Insert");
		lua_pushinteger(L, NB_KEY_DELETE); lua_setfield(L, -2, "Delete");
		lua_pushinteger(L, NB_KEY_RIGHT); lua_setfield(L, -2, "Right");
		lua_pushinteger(L, NB_KEY_LEFT); lua_setfield(L, -2, "Left");
		lua_pushinteger(L, NB_KEY_DOWN); lua_setfield(L, -2, "Down");
		lua_pushinteger(L, NB_KEY_UP); lua_setfield(L, -2, "Up");
		lua_pushinteger(L, NB_KEY_PAGE_UP); lua_setfield(L, -2, "PageUp");
		lua_pushinteger(L, NB_KEY_PAGE_DOWN); lua_setfield(L, -2, "PageDown");
		lua_pushinteger(L, NB_KEY_HOME); lua_setfield(L, -2, "Home");
		lua_pushinteger(L, NB_KEY_END); lua_setfield(L, -2, "End");
		lua_pushinteger(L, NB_KEY_CAPS_LOCK); lua_setfield(L, -2, "CapsLock");
		lua_pushinteger(L, NB_KEY_SCROLL_LOCK); lua_setfield(L, -2, "ScrollLock");
		lua_pushinteger(L, NB_KEY_NUM_LOCK); lua_setfield(L, -2, "NumLock");
		lua_pushinteger(L, NB_KEY_PRINT_SCREEN); lua_setfield(L, -2, "PrintScreen");
		lua_pushinteger(L, NB_KEY_PAUSE); lua_setfield(L, -2, "Pause");
		lua_pushinteger(L, NB_KEY_F1); lua_setfield(L, -2, "F1");
		lua_pushinteger(L, NB_KEY_F2); lua_setfield(L, -2, "F2");
		lua_pushinteger(L, NB_KEY_F3); lua_setfield(L, -2, "F3");
		lua_pushinteger(L, NB_KEY_F4); lua_setfield(L, -2, "F4");
		lua_pushinteger(L, NB_KEY_F5); lua_setfield(L, -2, "F5");
		lua_pushinteger(L, NB_KEY_F6); lua_setfield(L, -2, "F6");
		lua_pushinteger(L, NB_KEY_F7); lua_setfield(L, -2, "F7");
		lua_pushinteger(L, NB_KEY_F8); lua_setfield(L, -2, "F8");
		lua_pushinteger(L, NB_KEY_F9); lua_setfield(L, -2, "F9");
		lua_pushinteger(L, NB_KEY_F10); lua_setfield(L, -2, "F10");
		lua_pushinteger(L, NB_KEY_F11); lua_setfield(L, -2, "F11");
		lua_pushinteger(L, NB_KEY_F12); lua_setfield(L, -2, "F12");
		lua_pushinteger(L, NB_KEY_KP_0); lua_setfield(L, -2, "Keypad0");
		lua_pushinteger(L, NB_KEY_KP_1); lua_setfield(L, -2, "Keypad1");
		lua_pushinteger(L, NB_KEY_KP_2); lua_setfield(L, -2, "Keypad2");
		lua_pushinteger(L, NB_KEY_KP_3); lua_setfield(L, -2, "Keypad3");
		lua_pushinteger(L, NB_KEY_KP_4); lua_setfield(L, -2, "Keypad4");
		lua_pushinteger(L, NB_KEY_KP_5); lua_setfield(L, -2, "Keypad5");
		lua_pushinteger(L, NB_KEY_KP_6); lua_setfield(L, -2, "Keypad6");
		lua_pushinteger(L, NB_KEY_KP_7); lua_setfield(L, -2, "Keypad7");
		lua_pushinteger(L, NB_KEY_KP_8); lua_setfield(L, -2, "Keypad8");
		lua_pushinteger(L, NB_KEY_KP_9); lua_setfield(L, -2, "Keypad9");
		lua_pushinteger(L, NB_KEY_KP_DECIMAL); lua_setfield(L, -2, "KeypadDecimal");
		lua_pushinteger(L, NB_KEY_KP_DIVIDE); lua_setfield(L, -2, "KeypadDivide");
		lua_pushinteger(L, NB_KEY_KP_MULTIPLY); lua_setfield(L, -2, "KeypadMultiply");
		lua_pushinteger(L, NB_KEY_KP_SUBTRACT); lua_setfield(L, -2, "KeypadSubtract");
		lua_pushinteger(L, NB_KEY_KP_ADD); lua_setfield(L, -2, "KeypadAdd");
		lua_pushinteger(L, NB_KEY_KP_ENTER); lua_setfield(L, -2, "KeypadEnter");
		lua_pushinteger(L, NB_KEY_KP_EQUAL); lua_setfield(L, -2, "KeypadEqual");
		lua_pushinteger(L, NB_KEY_LEFT_SHIFT); lua_setfield(L, -2, "LeftShift");
		lua_pushinteger(L, NB_KEY_LEFT_CONTROL); lua_setfield(L, -2, "LeftControl");
		lua_pushinteger(L, NB_KEY_LEFT_ALT); lua_setfield(L, -2, "LeftAlt");
		lua_pushinteger(L, NB_KEY_LEFT_SUPER); lua_setfield(L, -2, "LeftSuper");
		lua_pushinteger(L, NB_KEY_RIGHT_SHIFT); lua_setfield(L, -2, "RightShift");
		lua_pushinteger(L, NB_KEY_RIGHT_CONTROL); lua_setfield(L, -2, "RightControl");
		lua_pushinteger(L, NB_KEY_RIGHT_ALT); lua_setfield(L, -2, "RightAlt");
		lua_pushinteger(L, NB_KEY_RIGHT_SUPER); lua_setfield(L, -2, "RightSuper");
		lua_pushinteger(L, NB_KEY_MENU); lua_setfield(L, -2, "Menu");

		// Register all mouse button constants
		lua_pushinteger(L, NB_MOUSE_BUTTON_LEFT); lua_setfield(L, -2, "MouseLeft");
		lua_pushinteger(L, NB_MOUSE_BUTTON_RIGHT); lua_setfield(L, -2, "MouseRight");
		lua_pushinteger(L, NB_MOUSE_BUTTON_MIDDLE); lua_setfield(L, -2, "MouseMiddle");
		lua_pushinteger(L, NB_MOUSE_BUTTON_4); lua_setfield(L, -2, "Mouse4");
		lua_pushinteger(L, NB_MOUSE_BUTTON_5); lua_setfield(L, -2, "Mouse5");
		lua_pushinteger(L, NB_MOUSE_BUTTON_6); lua_setfield(L, -2, "Mouse6");
		lua_pushinteger(L, NB_MOUSE_BUTTON_7); lua_setfield(L, -2, "Mouse7");
		lua_pushinteger(L, NB_MOUSE_BUTTON_8); lua_setfield(L, -2, "Mouse8");
	}

	void LuaBindings::RegisterLogBindings(lua_State* L)
	{
		// Register log functions
		lua_pushcfunction(L, LuaBindings::LogInfo);
		lua_setfield(L, -2, "Log");

		lua_pushcfunction(L, LuaBindings::LogWarn);
		lua_setfield(L, -2, "LogWarn");

		lua_pushcfunction(L, LuaBindings::LogError);
		lua_setfield(L, -2, "LogError");
	}

	void LuaBindings::RegisterMathBindings(lua_State* L)
	{
		// Math bindings can be added here
	}

	// Entity Transform Methods
	int LuaBindings::Entity_GetPosition(lua_State* L)
	{
		Entity* entity = (Entity*)lua_touserdata(L, 1);
		if (!entity)
		{
			luaL_error(L, "Invalid entity");
			return 0;
		}

		auto& transform = entity->GetComponent<TransformComponent>();
		lua_newtable(L);
		lua_pushnumber(L, transform.Position.x); lua_setfield(L, -2, "x");
		lua_pushnumber(L, transform.Position.y); lua_setfield(L, -2, "y");
		lua_pushnumber(L, transform.Position.z); lua_setfield(L, -2, "z");
		return 1;
	}

	int LuaBindings::Entity_SetPosition(lua_State* L)
	{
		Entity* entity = (Entity*)lua_touserdata(L, 1);
		if (!entity)
		{
			luaL_error(L, "Invalid entity");
			return 0;
		}

		float x = (float)lua_tonumber(L, 2);
		float y = (float)lua_tonumber(L, 3);
		float z = (float)lua_tonumber(L, 4);

		auto& transform = entity->GetComponent<TransformComponent>();
		transform.Position = { x, y, z };
		return 0;
	}

	int LuaBindings::Entity_GetRotation(lua_State* L)
	{
		Entity* entity = (Entity*)lua_touserdata(L, 1);
		if (!entity)
		{
			luaL_error(L, "Invalid entity");
			return 0;
		}

		auto& transform = entity->GetComponent<TransformComponent>();
		lua_newtable(L);
		lua_pushnumber(L, transform.Rotation.x); lua_setfield(L, -2, "x");
		lua_pushnumber(L, transform.Rotation.y); lua_setfield(L, -2, "y");
		lua_pushnumber(L, transform.Rotation.z); lua_setfield(L, -2, "z");
		return 1;
	}

	int LuaBindings::Entity_SetRotation(lua_State* L)
	{
		Entity* entity = (Entity*)lua_touserdata(L, 1);
		if (!entity)
		{
			luaL_error(L, "Invalid entity");
			return 0;
		}

		float x = (float)lua_tonumber(L, 2);
		float y = (float)lua_tonumber(L, 3);
		float z = (float)lua_tonumber(L, 4);

		auto& transform = entity->GetComponent<TransformComponent>();
		transform.Rotation = { x, y, z };
		return 0;
	}

	int LuaBindings::Entity_GetScale(lua_State* L)
	{
		Entity* entity = (Entity*)lua_touserdata(L, 1);
		if (!entity)
		{
			luaL_error(L, "Invalid entity");
			return 0;
		}

		auto& transform = entity->GetComponent<TransformComponent>();
		lua_newtable(L);
		lua_pushnumber(L, transform.Scale.x); lua_setfield(L, -2, "x");
		lua_pushnumber(L, transform.Scale.y); lua_setfield(L, -2, "y");
		lua_pushnumber(L, transform.Scale.z); lua_setfield(L, -2, "z");
		return 1;
	}

	int LuaBindings::Entity_SetScale(lua_State* L)
	{
		Entity* entity = (Entity*)lua_touserdata(L, 1);
		if (!entity)
		{
			luaL_error(L, "Invalid entity");
			return 0;
		}

		float x = (float)lua_tonumber(L, 2);
		float y = (float)lua_tonumber(L, 3);
		float z = (float)lua_tonumber(L, 4);

		auto& transform = entity->GetComponent<TransformComponent>();
		transform.Scale = { x, y, z };
		return 0;
	}

	// Log Functions
	int LuaBindings::LogInfo(lua_State* L)
	{
		const char* message = lua_tostring(L, 1);
		if (message)
			NB_CORE_INFO("[Lua] {0}", message);
		return 0;
	}

	int LuaBindings::LogWarn(lua_State* L)
	{
		const char* message = lua_tostring(L, 1);
		if (message)
			NB_CORE_WARN("[Lua] {0}", message);
		return 0;
	}

	int LuaBindings::LogError(lua_State* L)
	{
		const char* message = lua_tostring(L, 1);
		if (message)
			NB_CORE_ERROR("[Lua] {0}", message);
		return 0;
	}

	// Physics/RigidBody Functions
	int LuaBindings::Entity_AddForce(lua_State* L)
	{
		Entity* entity = (Entity*)lua_touserdata(L, 1);
		if (!entity || !entity->HasComponent<RigidBodyComponent>())
		{
			luaL_error(L, "Entity does not have RigidBodyComponent");
			return 0;
		}

		float x = (float)lua_tonumber(L, 2);
		float y = (float)lua_tonumber(L, 3);
		float z = (float)lua_tonumber(L, 4);

		auto& rb = entity->GetComponent<RigidBodyComponent>();
		if (rb.RuntimeBody)
		{
			btRigidBody* body = static_cast<btRigidBody*>(rb.RuntimeBody);
			body->applyCentralForce(btVector3(x, y, z));
			body->activate();
		}
		return 0;
	}

	int LuaBindings::Entity_SetVelocity(lua_State* L)
	{
		Entity* entity = (Entity*)lua_touserdata(L, 1);
		if (!entity || !entity->HasComponent<RigidBodyComponent>())
		{
			luaL_error(L, "Entity does not have RigidBodyComponent");
			return 0;
		}

		float x = (float)lua_tonumber(L, 2);
		float y = (float)lua_tonumber(L, 3);
		float z = (float)lua_tonumber(L, 4);

		auto& rb = entity->GetComponent<RigidBodyComponent>();
		if (rb.RuntimeBody)
		{
			btRigidBody* body = static_cast<btRigidBody*>(rb.RuntimeBody);
			body->setLinearVelocity(btVector3(x, y, z));
			body->activate();
		}
		return 0;
	}

	int LuaBindings::Entity_GetVelocity(lua_State* L)
	{
		Entity* entity = (Entity*)lua_touserdata(L, 1);
		if (!entity || !entity->HasComponent<RigidBodyComponent>())
		{
			luaL_error(L, "Entity does not have RigidBodyComponent");
			return 0;
		}

		auto& rb = entity->GetComponent<RigidBodyComponent>();
		if (rb.RuntimeBody)
		{
			btRigidBody* body = static_cast<btRigidBody*>(rb.RuntimeBody);
			btVector3 velocity = body->getLinearVelocity();
			lua_newtable(L);
			lua_pushnumber(L, velocity.x()); lua_setfield(L, -2, "x");
			lua_pushnumber(L, velocity.y()); lua_setfield(L, -2, "y");
			lua_pushnumber(L, velocity.z()); lua_setfield(L, -2, "z");
			return 1;
		}
		lua_pushnil(L);
		return 1;
	}

	int LuaBindings::Entity_SetAngularVelocity(lua_State* L)
	{
		Entity* entity = (Entity*)lua_touserdata(L, 1);
		if (!entity || !entity->HasComponent<RigidBodyComponent>())
		{
			luaL_error(L, "Entity does not have RigidBodyComponent");
			return 0;
		}

		float x = (float)lua_tonumber(L, 2);
		float y = (float)lua_tonumber(L, 3);
		float z = (float)lua_tonumber(L, 4);

		auto& rb = entity->GetComponent<RigidBodyComponent>();
		if (rb.RuntimeBody)
		{
			btRigidBody* body = static_cast<btRigidBody*>(rb.RuntimeBody);
			body->setAngularVelocity(btVector3(x, y, z));
			body->activate();
		}
		return 0;
	}

	int LuaBindings::Entity_GetAngularVelocity(lua_State* L)
	{
		Entity* entity = (Entity*)lua_touserdata(L, 1);
		if (!entity || !entity->HasComponent<RigidBodyComponent>())
		{
			luaL_error(L, "Entity does not have RigidBodyComponent");
			return 0;
		}

		auto& rb = entity->GetComponent<RigidBodyComponent>();
		if (rb.RuntimeBody)
		{
			btRigidBody* body = static_cast<btRigidBody*>(rb.RuntimeBody);
			btVector3 velocity = body->getAngularVelocity();
			lua_newtable(L);
			lua_pushnumber(L, velocity.x()); lua_setfield(L, -2, "x");
			lua_pushnumber(L, velocity.y()); lua_setfield(L, -2, "y");
			lua_pushnumber(L, velocity.z()); lua_setfield(L, -2, "z");
			return 1;
		}
		lua_pushnil(L);
		return 1;
	}

	int LuaBindings::Entity_SetMass(lua_State* L)
	{
		Entity* entity = (Entity*)lua_touserdata(L, 1);
		if (!entity || !entity->HasComponent<RigidBodyComponent>())
		{
			luaL_error(L, "Entity does not have RigidBodyComponent");
			return 0;
		}

		float mass = (float)lua_tonumber(L, 2);
		auto& rb = entity->GetComponent<RigidBodyComponent>();
		rb.Mass = mass;

		if (rb.RuntimeBody)
		{
			btRigidBody* body = static_cast<btRigidBody*>(rb.RuntimeBody);
			btVector3 inertia(0, 0, 0);
			if (mass > 0.0f)
				body->getCollisionShape()->calculateLocalInertia(mass, inertia);
			body->setMassProps(mass, inertia);
		}
		return 0;
	}

	int LuaBindings::Entity_GetMass(lua_State* L)
	{
		Entity* entity = (Entity*)lua_touserdata(L, 1);
		if (!entity || !entity->HasComponent<RigidBodyComponent>())
		{
			luaL_error(L, "Entity does not have RigidBodyComponent");
			return 0;
		}

		auto& rb = entity->GetComponent<RigidBodyComponent>();
		lua_pushnumber(L, rb.Mass);
		return 1;
	}

	void LuaBindings::RegisterRigidBodyComponent(lua_State* L)
	{
		// This is called from RegisterEngineFunctions in LuaScriptEngine
		// Physics methods are registered as part of the Entity metatable
	}

}

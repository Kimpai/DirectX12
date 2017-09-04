#pragma once

#include <lua.hpp>
#include <lauxlib.h>

#pragma comment(lib, "lua5.3.lib")
#pragma comment(lib, "lua5.3-static.lib")
#pragma comment(lib, "luac.lib")

class LuaClass
{
public:
	LuaClass();
	LuaClass(const LuaClass&);
	~LuaClass();

	void Initialize();
	void Shutdown();
	void PushCFunction(lua_CFunction);
	void SetGlobal(char*);

private:
	lua_State* m_luaState;
};
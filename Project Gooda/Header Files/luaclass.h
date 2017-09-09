#pragma once

#include <lua.hpp>
#include <lauxlib.h>
#include <string>
#include <iostream>

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
	bool DoString(const char*);
	lua_State* GetState();

	static int Print(lua_State* L);

private:
	lua_State* m_luaState;
};
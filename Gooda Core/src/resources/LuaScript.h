//#pragma once
//
//#include <lua.hpp>
//#include <lauxlib.h>
//#include <string>
//#include <iostream>
//
//class Lua
//{
//public:
//	Lua();
//	Lua(const Lua&);
//	~Lua();
//
//	void Initialize();
//	void Shutdown();
//	void PushCFunction(lua_CFunction);
//	void SetGlobal(char*);
//	bool DoString(const char*);
//	lua_State* GetState();
//
//	static int Print(lua_State* L);
//
//private:
//	lua_State* m_luaState;
//};
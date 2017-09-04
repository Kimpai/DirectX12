#include "luaclass.h"

LuaClass::LuaClass()
{
	m_luaState = nullptr;
}

LuaClass::LuaClass(const LuaClass& other)
{

}

LuaClass::~LuaClass()
{

}

void LuaClass::Initialize()
{
	m_luaState = luaL_newstate();

	return;
}

void LuaClass::Shutdown()
{
	if (m_luaState)
	{
		m_luaState = nullptr;
	}

	return;
}

void LuaClass::PushCFunction(lua_CFunction luaCFunction)
{
	lua_pushcfunction(m_luaState, luaCFunction);

	return;
}

void LuaClass::SetGlobal(char* luaCFunction)
{
	lua_setglobal(m_luaState, luaCFunction);

	return;
}

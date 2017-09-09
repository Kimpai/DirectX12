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



	luaL_openlibs(m_luaState);

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

bool LuaClass::DoString(const char* input)
{
	if (luaL_dostring(m_luaState, input) != false)
		return false;

	return true;
}

lua_State* LuaClass::GetState()
{
	return m_luaState;
}

int LuaClass::Print(lua_State* L)
{
	
	int n = lua_gettop(L);

	if (n > 0 && lua_isnumber(L, 1))
	{
		std::cout << lua_tonumber(L, 1) << std::endl;
	}
	else
	{
			std::cout << "No number" << std::endl;
	}

	return 0;
}

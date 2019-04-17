//#include "LuaScript.h"
//
//Lua::Lua()
//{
//	m_luaState = nullptr;
//}
//
//Lua::Lua(const Lua& other)
//{
//
//}
//
//Lua::~Lua()
//{
//
//}
//
//void Lua::Initialize()
//{
//	m_luaState = luaL_newstate();
//
//
//
//	luaL_openlibs(m_luaState);
//
//	return;
//}
//
//void Lua::Shutdown()
//{
//	if (m_luaState)
//	{
//		m_luaState = nullptr;
//	}
//
//	return;
//}
//
//void Lua::PushCFunction(lua_CFunction luaCFunction)
//{
//	lua_pushcfunction(m_luaState, luaCFunction);
//
//	return;
//}
//
//void Lua::SetGlobal(char* luaCFunction)
//{
//	lua_setglobal(m_luaState, luaCFunction);
//
//	return;
//}
//
//bool Lua::DoString(const char* input)
//{
//	if (luaL_dostring(m_luaState, input) != false)
//		return false;
//
//	return true;
//}
//
//lua_State* Lua::GetState()
//{
//	return m_luaState;
//}
//
//int Lua::Print(lua_State* L)
//{
//	
//	int n = lua_gettop(L);
//
//	if (n > 0 && lua_isnumber(L, 1))
//	{
//		std::cout << lua_tonumber(L, 1) << std::endl;
//	}
//	else
//	{
//			std::cout << "No number" << std::endl;
//	}
//
//	return 0;
//}

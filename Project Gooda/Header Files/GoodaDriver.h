#pragma once

#include <Windows.h>
#include <lua.hpp>
#include <lauxlib.h>
#include <config.h>
#include <iostream>

#include "Direct3D.h"
#include "Colorshader.h"
#include "Cube.h"
#include "Camera.h"

static bool renderQuad = true;

class GoodaDriver
{
public:
	GoodaDriver();
	GoodaDriver(const GoodaDriver&);
	~GoodaDriver();

	void Initialize(HWND, Camera*);
	void Shutdown();
	void Frame(Camera*);

	static int RenderQuad(lua_State* L);

private:
	void Render();

	Direct3D* m_Direct3D;
	ColorShader* m_ColorShader;
	Cube* m_Cube;
};
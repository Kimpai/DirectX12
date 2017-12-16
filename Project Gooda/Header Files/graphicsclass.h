#pragma once

#include <Windows.h>
#include <lua.hpp>
#include <lauxlib.h>
#include <iostream>
#include "d3dclass.h"
#include "colorshaderclass.h"
#include "cubeclass.h"
#include "cameraclass.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
static bool renderQuad = true;

class GoodaDriver
{
public:
	GoodaDriver();
	GoodaDriver(const GoodaDriver&);
	~GoodaDriver();

	bool Initialize(int, int, HWND, Camera*);
	void Shutdown();
	bool Frame(Camera*);

	static int RenderQuad(lua_State* L);

private:
	bool Render();

	Direct3D* m_Direct3D;
	ColorShader* m_ColorShader;
	Cube* m_Cube;
};
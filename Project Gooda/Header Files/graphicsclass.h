#pragma once

#include <Windows.h>
#include <lua.hpp>
#include <lauxlib.h>
#include <iostream>
#include "d3dclass.h"
#include "colorshaderclass.h"
#include "cubeclass.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
static bool renderQuad = true;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

	static int RenderQuad(lua_State* L);

private:
	bool Render();

	Direct3DClass* m_Direct3D;
	ColorShaderClass* m_ColorShader;
	CubeClass* m_Cube;
};
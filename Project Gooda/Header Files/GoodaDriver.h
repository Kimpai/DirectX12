#pragma once

#include <Windows.h>
#include <lua.hpp>
#include <lauxlib.h>
#include <config.h>
#include <iostream>
#include <vector>

#include "Direct3D.h"
#include "Colorshader.h"
#include "Cube.h"
#include "Terrain.h"
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

private:
	void Render();

	Direct3D* m_Direct3D;
	ColorShader* m_ColorShader;
	std::vector<Model*> m_Models;
	
};
#pragma once

#include <Windows.h>
#include <lua.hpp>
#include <lauxlib.h>
#include <config.h>
#include <iostream>
#include <vector>

#include "Direct3D.h"
#include "Shader.h"
#include "Cube.h"
#include "Terrain.h"
#include "Camera.h"
#include "DirectionalLight.h"

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
	Shader* m_Shader;
	std::vector<Model*> m_Models;
	std::vector<Light*> m_Lights;
};
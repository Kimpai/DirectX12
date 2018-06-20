#pragma once

#include <Windows.h>
#include <config.h>
#include <iostream>
#include <vector>

#include "Direct3D.h"
#include "ShaderManager.h"
#include "Cube.h"
#include "Terrain.h"
#include "Camera.h"
#include "DirectionalLight.h"

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
	ShaderManager* m_Shader;
	std::vector<Model*> m_Models;
	std::vector<Light*> m_Lights;
};
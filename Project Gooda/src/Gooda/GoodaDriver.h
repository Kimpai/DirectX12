#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>
#include <Gooda.h>

#include "Camera.h"
#include "Direct3D12.h"
#include "ShaderManager.h"
#include "Cube.h"
#include "Terrain.h"
#include "Camera.h"
#include "DirectionalLight.h"

class GoodaDriver : public Gooda
{
public:
	GoodaDriver(HWND hwnd, Input*);
	~GoodaDriver();

	void Frame();

private:
	void Render();

	Camera* m_camera;
	Direct3D* m_direct3D;
	ShaderManager* m_shaderManager;
	std::vector<Model*> m_models;
	std::vector<Light*> m_lights;
};
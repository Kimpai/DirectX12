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
#include "SoundManager.h"

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
	SoundManager* m_Sound;
	std::vector<int> m_Channels;
	std::vector<Model*> m_Models;
	std::vector<Light*> m_Lights;
};
#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>
#include "Gooda.h"

#include "../graphics/Camera.h"
#include "../graphics/RenderManager.h"
#include "../graphics/ShaderManager.h"
#include "../graphics/Cube.h"
#include "../graphics/Terrain.h"
#include "../graphics/DirectionalLight.h"

namespace GoodaCore
{
	class GoodaDriver : public Gooda
	{
	public:
		GoodaDriver(HWND hwnd, Input*);
		virtual ~GoodaDriver();

		void Frame();

	private:
		void Render();

		Camera* m_camera;
		std::vector<Model*> m_models;
		std::vector<Light*> m_lights;
	};
}
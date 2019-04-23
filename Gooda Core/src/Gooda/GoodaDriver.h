#pragma once

#pragma comment(lib, "Gooda.lib")

#include <iostream>
#include <vector>

#include "../graphics/Window.h"
#include "../graphics/Camera.h"
#include "../graphics/ShaderManager.h"
#include "../graphics/Cube.h"
#include "../graphics/Renderer.h"
#include "../graphics/DirectionalLight.h"

namespace GoodaCore
{
	class GoodaDriver : public Gooda
	{
	public:
		virtual ~GoodaDriver() = default;
		static GoodaDriver* Instance();

		virtual bool Init();
		virtual bool Frame();
		virtual bool Destroy();

	private:
		GoodaDriver() = default;

		Camera* m_camera;
		Window* m_window;
		std::vector<Model*> m_models;
		std::vector<Light*> m_lights;
	};
}
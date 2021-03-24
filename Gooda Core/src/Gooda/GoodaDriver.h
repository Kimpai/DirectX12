#pragma once

#include <iostream>
#include <vector>

#include "Window.h"
#include "../graphics/Camera.h"
#include "../utils/ShaderManager.h"
#include "../resources/Cube.h"
#include "../resources/Plane.h"
#include "../graphics/Renderer.h"
#include "../graphics/DirectionalLight.h"
#include "Console.h"

namespace GoodaCore
{
	class GoodaDriver : public Gooda
	{
	public:
		virtual ~GoodaDriver() = default;
		static GoodaDriver* Instance();

		virtual bool Init();
		virtual bool Run();
		virtual bool Destroy();

	private:
		GoodaDriver() = default;

		virtual bool Frame();

		Camera* m_camera;
		std::vector<Model*> m_models;
		std::vector<Light*> m_lights;
	};
}
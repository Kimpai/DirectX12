#pragma once

#include <assert.h>
#include <frame.h>
#include <Gooda.h>
#include <memory>

#include "Renderer.h"

namespace GoodaCore
{
	class RenderManager : public Gooda
	{
	public:
		RenderManager() = default;
		virtual ~RenderManager() = default;
	};
}
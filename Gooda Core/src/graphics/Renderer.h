#pragma once

#include <assert.h>
#include <frame.h>
#include <memory>

#include "Window.h"
#include "Model.h"
#include "Light.h"
#include "ShaderManager.h"
#include "../utils/DX11/Direct3D11.h"
#include "../utils/DX12/Direct3D12.h"

using namespace Microsoft::WRL;

namespace GoodaCore
{
	enum class Backend { DX11, DX12, NOACTIVE };

	class Renderer : public Gooda
	{
	public:
		virtual ~Renderer() = default;

		virtual bool Init();
		virtual bool Destroy();

		static Renderer* Instance();
		Backend GetBackend();

		bool Render(std::vector<Model*>&);
		bool BeginScene(Window*);
		bool EndScene(Window*);

	private:
		Renderer();

	private:
		Backend m_backend;
	};
}
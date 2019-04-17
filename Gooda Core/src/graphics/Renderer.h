#pragma once

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include <assert.h>
#include <frame.h>
#include <Gooda.h>
#include <memory>

#include "../utils/DX11/Direct3D11.h"
#include "../utils/DX12/Direct3D12.h"

using namespace Microsoft::WRL;

namespace GoodaCore
{
	enum class Backend { DX11, DX12, NOACTIVE };

	class Renderer : Gooda
	{
	public:
		virtual ~Renderer();

		static Renderer* Instance();

		void* GetDevice();
		void* GetDeviceContextOrCommandList();
		Backend GetBackend();
		int GetCurrentFrame();

		void Render();
	private:
		Renderer();
		Backend m_backend;
	};
}
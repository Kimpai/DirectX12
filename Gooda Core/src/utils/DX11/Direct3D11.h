#pragma once

#pragma comment(lib, "d3d11.lib")

#include <d3d11.h>
#include <dxgi1_5.h>
#include <dxgidebug.h>
#include <wrl.h>
#include <assert.h>
#include <frame.h>

#include "../../Gooda/Gooda.h"

using namespace Microsoft::WRL;

namespace GoodaCore
{
	class Direct3D11 : public Gooda
	{
	public:
		~Direct3D11();

		static Direct3D11* Instance();

		void BeginScene();
		void EndScene();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();
		int GetCurrentFrame();

	private:
		Direct3D11();

		void CreateDirect3DDeviceAndSwapChain();
		void CreateViewPortAndScissorRect();
		void CreateRenderTargets();

		UINT m_frameIndex;

		ComPtr<ID3D11Device> m_device;
		ComPtr<ID3D11DeviceContext> m_deviceContext;
		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		D3D11_VIEWPORT m_viewport;
		D3D11_RECT m_rect;
		ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
		ComPtr<ID3D11DepthStencilState> m_depthStencilState;
		ComPtr<ID3D11DepthStencilView> m_depthStencilView;
		ComPtr<ID3D11RasterizerState> m_rasterState;
	};
}
#pragma once

#pragma comment(lib, "d3d11.lib")

#include <d3d11.h>
#include <dxgi1_5.h>
#include <dxgidebug.h>
#include <wrl.h>
#include <assert.h>
#include <frame.h>
#include <Gooda.h>

using namespace Microsoft::WRL;

class Direct3D11 : public Gooda
{
public:
	Direct3D11(HWND);
	~Direct3D11();

	void BeginScene();
	void EndScene();
	
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	int GetCurrentFrame();

private:
	void CreateDirect3DDeviceAndSwapChain(HWND);
	void CreateViewPortAndScissorRect();
	void CreateRenderTargets();

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
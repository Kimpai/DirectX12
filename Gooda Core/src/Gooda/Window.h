#pragma once

#include <Windows.h>
#include <string>

#include "../utils/DX12/Direct3D12.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

namespace GoodaCore
{
	class Window : public Gooda
	{
	public:
		virtual ~Window() = default;
		virtual bool BeginScene();
		static Window* Instance();

		bool Init();
		bool Destroy();

		UINT GetCurrentFrameIndex();
		HWND GetWindowHandle();

		bool EndScene();

	protected:
		struct Resources
		{
			ID3D12Resource* m_depthStencilBuffer;
			ID3D12DescriptorHeap* m_depthStencilDescHeap;
			ID3D12DescriptorHeap* m_renderTargetViewDescHeap;
			ID3D12Resource* m_backBufferRenderTargetView[frameBufferCount];

		} m_resources;

	private:
		Window() = default;
		bool CreateViewPortAndScissorRect(UINT, UINT, FLOAT, FLOAT);

		IDXGISwapChain3* m_swapChain;
		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_rect;
		HINSTANCE m_hinstance;
		HWND m_windowHandle;
		WNDCLASSEX m_wc;
	};
	
}
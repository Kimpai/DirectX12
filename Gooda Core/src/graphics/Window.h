#pragma once

#include <Windows.h>
#include <string>

#include "../utils/DX12/Direct3D12.h"

namespace GoodaCore
{
	class Window : public Gooda
	{
	public:
		Window();
		virtual ~Window();
		virtual bool Frame();

		HWND GetWindowHandle();

		bool Present();

		static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	protected:
		struct Resources
		{
			ComPtr<ID3D12Resource> m_depthStencilBuffer;
			ComPtr<ID3D12DescriptorHeap> m_depthStencilDescHeap;
			ComPtr<ID3D12DescriptorHeap> m_renderTargetViewDescHeap;
			ComPtr<ID3D12Resource> m_backBufferRenderTargetView[frameBufferCount];

		} m_resources;

	private:
		bool CreateViewPortAndScissorRect(UINT, UINT, FLOAT, FLOAT);

		ComPtr<IDXGISwapChain3> m_swapChain;
		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_rect;
		HINSTANCE m_hinstance;
		HWND m_windowHandle;
		WNDCLASSEX m_wc;
	};
	
}
#pragma once

#include <Windows.h>
#include <string>

#include "../Gooda/Input.h"
#include "../utils/DX12/Direct3D12.h"

using namespace DirectX;

namespace GoodaCore
{
	class Window : public Gooda
	{
	public:
		Window() = default;
		virtual ~Window() = default;

		virtual bool Init();
		virtual bool Destroy();
		virtual bool Frame();

		UINT GetCurrentFrame();
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilViewHandle();
		HWND GetWindowHandle();

		bool Present();

	protected:
		struct Direct3D
		{
			ComPtr<ID3D12Resource> m_depthStencilBuffer;
			ComPtr<ID3D12DescriptorHeap> m_depthStencilDescHeap;
			ComPtr<ID3D12DescriptorHeap> m_renderTargetViewDescHeap;
			ComPtr<ID3D12Resource> m_backBufferRenderTargetView[frameBufferCount];

		} m_direct3D;

	private:
		bool CreateViewPortAndScissorRect(UINT, UINT, FLOAT, FLOAT);

		ComPtr<IDXGIOutput3> m_outputMonitor;
		ComPtr<IDXGISwapChain3> m_swapChain;

		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_rect;
		HINSTANCE m_hinstance;
		HWND m_windowHandle;
		UINT m_bufferIndex;
		UINT m_frameIndex;
		LPCSTR m_name;
	};
	
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
}
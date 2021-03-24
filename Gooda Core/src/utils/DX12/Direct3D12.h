#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <wrl.h>
#include <Frame.h>

#include "../../Gooda/Gooda.h"

using namespace Microsoft::WRL;

namespace GoodaCore
{
	class Direct3D12 : public Gooda
	{
	public:
		virtual ~Direct3D12() = default;
		static Direct3D12* Instance();

		virtual bool Init();
		virtual bool Destroy();

		bool BeginFrame(UINT);
		bool EndFrame(UINT);

		bool CreateSwapChainForHWND(UINT, UINT, HWND, IDXGISwapChain3**);

		bool CreateRenderTargets(IDXGISwapChain3*, ID3D12Resource**, ID3D12DescriptorHeap**);
		bool CreateBackBufferRenderTarget(IDXGISwapChain3*, ID3D12Resource**, ID3D12DescriptorHeap**, HWND);
		bool CreateDepthStencil(ID3D12Resource**, ID3D12DescriptorHeap**);

		bool UpdateBackBufferRenderTarget(IDXGISwapChain3*, ID3D12Resource**, ID3D12DescriptorHeap**, HWND);

		bool CloseCommandList(UINT);
		bool ResetCommandList(UINT);
		bool ExecuteCommandList(UINT);

		bool DeviceSynchronize(UINT);
		bool FlushCommandQueue();

		void TransitionResource(ID3D12Resource*, D3D12_RESOURCE_STATES, D3D12_RESOURCE_STATES);

		ID3D12Device* GetDevice();
		ID3D12GraphicsCommandList* GetCommandList();

	private:
		bool CreateDirect3DDevice();
		bool CreateCommandInterface();
		bool CreateFenceAndEventHandle();

		ID3D12Device* m_device;
		ID3D12CommandAllocator* m_commandAllocator[frameBufferCount];
		ID3D12GraphicsCommandList* m_commandList;
		ID3D12CommandQueue* m_commandQueue;
		ID3D12Fence* m_fence;
		IDXGIAdapter3* m_adapter;
		IDXGIDebug* m_dxgiDebug;

		HANDLE m_fenceEvent;
		UINT64 m_fenceValue;

		Direct3D12() = default;
	};
}
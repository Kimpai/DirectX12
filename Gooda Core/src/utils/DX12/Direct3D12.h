#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_5.h>
#include <wrl.h>
#include <assert.h>
#include <frame.h>
#include <Gooda.h>

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

		bool CreateSwapChainAndDisplayInterface(IDXGISwapChain3**, IDXGIOutput3**,
			UINT, UINT, HWND);
		bool CreateRenderTargets(ID3D12Resource**, ID3D12DescriptorHeap**);
		bool CreateBackBufferRenderTarget(IDXGISwapChain3*, ID3D12Resource**, ID3D12DescriptorHeap**);
		bool CreateDepthStencil(D3D12_DEPTH_STENCIL_DESC&, ID3D12Resource**, ID3D12DescriptorHeap**);

		bool CloseCommandList();
		bool ResetCommandList(UINT);
		bool ExecuteCommandList(UINT);

		bool DeviceSynchronize(UINT);
		bool FlushCommandQueue(UINT);

		ID3D12Device* GetDevice();
		ID3D12GraphicsCommandList* GetCommandList();

	private:
		Direct3D12();

		bool CreateDirect3DDevice();
		bool CreateCommandInterface();
		bool CreateFenceAndEventHandle();

	private:
		ComPtr<ID3D12Device> m_device;
		ComPtr<ID3D12CommandAllocator> m_commandAllocator[frameBufferCount];
		ComPtr<ID3D12GraphicsCommandList> m_commandList;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
		ComPtr<ID3D12Fence> m_fence[frameBufferCount];
		ComPtr<IDXGIAdapter3> m_GPUAdapter;

		HANDLE m_fenceEvent;
		UINT64 m_fenceValue[frameBufferCount];
	};
}
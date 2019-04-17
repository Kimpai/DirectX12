#pragma once

#pragma comment(lib, "d3d12.lib")

#include <d3d12.h>
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
		virtual ~Direct3D12();

		static Direct3D12* Instance();

		void Init(HWND);
		void BeginScene(D3D12_CPU_DESCRIPTOR_HANDLE*, D3D12_GPU_DESCRIPTOR_HANDLE*);
		void EndScene();
		void CloseCommandList();
		void ResetCommandList(ID3D12PipelineState*);
		void ExecuteCommandList();

		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilViewHandle();
		ID3D12Device* GetDevice();
		ID3D12GraphicsCommandList* GetCommandList();
		int GetCurrentFrame();

	private:
		Direct3D12();

		void DeviceSynchronize();
		void FlushCommandQueue();
		void CreateDirect3DDevice();
		void CreateViewPortAndScissorRect();
		void CreateFenceAndEventHandle();
		void CreateCommandInterfaceAndSwapChain(HWND);
		void CreateRenderTargets();
		void CreateDepthStencil(D3D12_DEPTH_STENCIL_DESC& depthStencilDesc);

	private:
		UINT m_bufferIndex;
		UINT m_frameIndex;

		ComPtr<ID3D12Device> m_device;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<ID3D12DescriptorHeap> m_renderTargetViewDescHeap;
		ComPtr<ID3D12Resource> m_backBufferRenderTarget[frameBufferCount];
		ComPtr<ID3D12CommandAllocator> m_commandAllocator[frameBufferCount];
		ComPtr<ID3D12GraphicsCommandList> m_commandList;
		ComPtr<ID3D12Fence> m_fence[frameBufferCount];
		ComPtr<ID3D12Resource> m_depthStencilBuffer;
		ComPtr<ID3D12DescriptorHeap> m_depthStencilDescHeap;

		HANDLE m_fenceEvent;
		UINT64 m_fenceValue[frameBufferCount];
		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_rect;
	};
}
#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include <d3d12.h>
#include <dxgi1_5.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <assert.h>
#include <frame.h>
#include <Gooda.h>

#include "ShaderManager.h"

using namespace Microsoft::WRL;

class Direct3D12 : public Gooda
{
public:
	Direct3D12(HWND);
	~Direct3D12();

	void BeginScene(ShaderManager*);
	void EndScene();
	void CloseCommandList();
	void ResetCommandList(ID3D12PipelineState*);
	void ExecuteCommandList();

	ID3D12Device* GetDevice();
	ID3D12GraphicsCommandList* GetCommandList();
	int GetCurrentFrame();

private:
	void DeviceSynchronize();
	void FlushCommandQueue();
	void CreateDirect3DDevice(HWND);
	void CreateViewPortAndScissorRect();
	void CreateFenceAndEventHandle();
	void CreateCommandInterfaceAndSwapChain(HWND);
	void CreateRenderTargets();

private:
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	char m_videoCardDescription[128];
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12DescriptorHeap> m_renderTargetViewDescHeap;
	ComPtr<ID3D12Resource> m_backBufferRenderTarget[frameBufferCount];
	unsigned int m_bufferIndex;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator[frameBufferCount];
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12Fence> m_fence[frameBufferCount];
	HANDLE m_fenceEvent;
	UINT64 m_fenceValue[frameBufferCount];
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_rect;
	int m_frameIndex;
	int m_videoCardMemory;
};

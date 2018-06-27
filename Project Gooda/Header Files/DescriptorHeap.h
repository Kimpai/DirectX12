#pragma once
#include <d3dx12.h>
#include <wrl.h>
#include <assert.h>
#include <frame.h>

using namespace Microsoft::WRL;

class DescriptorHeap	
{
public:
	DescriptorHeap(ID3D12Device* device, UINT numOfDescriptors, bool shaderVisible, D3D12_DESCRIPTOR_HEAP_TYPE type);
	
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUIncrementHandle(int frameIndex, int descriptorIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUIncrementHandle(int frameIndex, int descriptorIndex);
	void AppendDescriptorToHeap(D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc);
	int GetDescriptorHeapSize();
	ID3D12DescriptorHeap* GetDescriptorHeap(int frameIndex);

private:
	UINT m_incrementSize;
	int m_size;
	ComPtr<ID3D12DescriptorHeap> m_descriptorHeap[frameBufferCount];
	ID3D12Device* m_device;
};
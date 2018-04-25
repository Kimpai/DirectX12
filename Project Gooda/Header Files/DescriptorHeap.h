#pragma once
#include <d3dx12.h>
#include <wrl.h>
#include <vector>
#include <assert.h>

using namespace Microsoft::WRL;


class DescriptorHeap	
{
public:
	DescriptorHeap(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12GraphicsCommandList* computeCommandList, const UINT & numHeaps);
	void CreateDescriptorHeap(const UINT & numDesc, D3D12_DESCRIPTOR_HEAP_FLAGS heapFlags, D3D12_DESCRIPTOR_HEAP_TYPE type);

public:
	void SetRootDescriptorTable(const UINT & rootIndex, D3D12_GPU_DESCRIPTOR_HANDLE handle, const UINT & frameIndex = 0);
	void SetRootDescriptorTable(const UINT & rootIndex, const UINT & frameIndex = 0);
	void SetComputeRootDescriptorTable(const UINT & rootIndex, const UINT & frameIndex = 0);
	void SetComputeRootDescriptorTable(const UINT & rootIndex, D3D12_GPU_DESCRIPTOR_HANDLE handle, const UINT & frameIndex = 0);

public:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUIncrementHandle(const INT & resourceIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUIncrementHandle(const INT & resourceIndex);
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> GetCPUIncrementHandleForMultipleHeaps(const INT & resourceIndex);

private:
	std::vector<ComPtr<ID3D12DescriptorHeap>> m_descHeaps;
	UINT m_handleIncrementSize;

private:
	ID3D12Device* m_device;
	ID3D12GraphicsCommandList* m_commandList;
	ID3D12GraphicsCommandList* m_computeCommandList;
};
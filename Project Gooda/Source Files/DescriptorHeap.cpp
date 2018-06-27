#include "DescriptorHeap.h"

DescriptorHeap::DescriptorHeap(ID3D12Device * device, UINT numOfDescriptors, bool shaderVisible, D3D12_DESCRIPTOR_HEAP_TYPE type) : m_device(device)
{
	for (int i = 0; i < frameBufferCount; ++i)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = numOfDescriptors;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS(shaderVisible);
		heapDesc.Type = type;
		assert(!m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_descriptorHeap[i])));

		m_descriptorHeap[i]->SetName(L"Descriptor Heap");
	}

	m_incrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_size = 0;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUIncrementHandle(int frameIndex, int descriptorIndex)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptorHeap[frameIndex]->GetCPUDescriptorHandleForHeapStart(), descriptorIndex, m_incrementSize);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUIncrementHandle(int frameIndex, int descriptorIndex)
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorHeap[frameIndex]->GetGPUDescriptorHandleForHeapStart(), descriptorIndex, m_incrementSize);;
}

void DescriptorHeap::AppendDescriptorToHeap(D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc)
{
	for (int i = 0; i < frameBufferCount; ++i)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_descriptorHeap[i]->GetCPUDescriptorHandleForHeapStart(), m_size, m_incrementSize);
		m_device->CreateConstantBufferView(&cbvDesc[i], handle);
	}

	++m_size;
}

int DescriptorHeap::GetDescriptorHeapSize()
{
	return m_size;
}

ID3D12DescriptorHeap* DescriptorHeap::GetDescriptorHeap(int frameIndex)
{
	return m_descriptorHeap[frameIndex].Get();
}

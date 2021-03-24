#include "DescriptorHeap.h"

namespace GoodaCore
{
	DescriptorHeap::DescriptorHeap(UINT numOfDescriptors, bool shaderVisible, D3D12_DESCRIPTOR_HEAP_TYPE type)
	{
		for (int i = 0; i < frameBufferCount; ++i)
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
			heapDesc.NumDescriptors = numOfDescriptors;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS(shaderVisible);
			heapDesc.Type = type;
			assert(!Direct3D12::Instance()->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_descriptorHeap[i])));

			m_descriptorHeap[i]->SetName(L"Descriptor Heap");
		}

		m_incrementSize = Direct3D12::Instance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_size = 0;
	}

	DescriptorHeap::~DescriptorHeap()
	{
		for (int i = 0; i < frameBufferCount; i++)
			m_descriptorHeap[i]->Release();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUIncrementHandle(UINT frameIndex, UINT descriptorIndex)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = m_descriptorHeap[frameIndex]->GetCPUDescriptorHandleForHeapStart().ptr + (descriptorIndex * m_incrementSize);

		return handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUIncrementHandle(UINT frameIndex, UINT descriptorIndex)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = m_descriptorHeap[frameIndex]->GetGPUDescriptorHandleForHeapStart().ptr + (descriptorIndex * m_incrementSize);

		return handle;
	}

	void DescriptorHeap::AppendDescriptorToHeap(D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc)
	{
		for (int i = 0; i < frameBufferCount; i++)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_descriptorHeap[i]->GetCPUDescriptorHandleForHeapStart(), m_size, m_incrementSize);
			Direct3D12::Instance()->GetDevice()->CreateConstantBufferView(&cbvDesc[i], handle);
		}

		m_size++;
	}

	int DescriptorHeap::GetDescriptorHeapSize()
	{
		return m_size;
	}

	ID3D12DescriptorHeap* DescriptorHeap::GetDescriptorHeap(UINT frameIndex)
	{
		return m_descriptorHeap[frameIndex];
	}
}
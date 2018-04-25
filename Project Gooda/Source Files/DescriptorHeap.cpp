#include "DescriptorHeap.h"

DescriptorHeap::DescriptorHeap(ID3D12Device * device, ID3D12GraphicsCommandList * commandList, ID3D12GraphicsCommandList * computeCommandList, const UINT & numHeaps)
{

}

void DescriptorHeap::CreateDescriptorHeap(const UINT & numDesc, D3D12_DESCRIPTOR_HEAP_FLAGS heapFlags, D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	for (UINT i = 0; i < m_descHeaps.size(); ++i)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = numDesc;
		heapDesc.Flags = heapFlags;
		heapDesc.Type = type;

		//Create the descriptor heap from the description
		assert(!m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_descHeaps[i].GetAddressOf())));
	}

	//Store increment size of the desc type
	m_handleIncrementSize = m_device->GetDescriptorHandleIncrementSize(type);
}

void DescriptorHeap::SetRootDescriptorTable(const UINT & rootIndex, D3D12_GPU_DESCRIPTOR_HANDLE handle, const UINT & frameIndex)
{
	if (m_descHeaps.size() > 1)
	{
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_descHeaps[frameIndex].Get() };
		m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		m_commandList->SetGraphicsRootDescriptorTable(rootIndex, m_descHeaps[frameIndex]->GetGPUDescriptorHandleForHeapStart());
	}
	else
	{
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_descHeaps[0].Get() };
		m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		m_commandList->SetGraphicsRootDescriptorTable(rootIndex, handle);
	}
}

void DescriptorHeap::SetRootDescriptorTable(const UINT & rootIndex, const UINT & frameIndex)
{
	if (m_descHeaps.size() > 1)
	{
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_descHeaps[frameIndex].Get() };
		m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		m_commandList->SetGraphicsRootDescriptorTable(rootIndex, m_descHeaps[frameIndex]->GetGPUDescriptorHandleForHeapStart());
	}
	else
	{
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_descHeaps[0].Get() };
		m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		m_commandList->SetGraphicsRootDescriptorTable(rootIndex, m_descHeaps[0]->GetGPUDescriptorHandleForHeapStart());
	}
}

void DescriptorHeap::SetComputeRootDescriptorTable(const UINT & rootIndex, const UINT & frameIndex)
{
	if (m_descHeaps.size() > 1)
	{
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_descHeaps[frameIndex].Get() };
		m_computeCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		m_computeCommandList->SetComputeRootDescriptorTable(rootIndex, m_descHeaps[frameIndex]->GetGPUDescriptorHandleForHeapStart());
	}
	else
	{
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_descHeaps[0].Get() };
		m_computeCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		m_computeCommandList->SetComputeRootDescriptorTable(rootIndex, m_descHeaps[0]->GetGPUDescriptorHandleForHeapStart());
	}
}

void DescriptorHeap::SetComputeRootDescriptorTable(const UINT & rootIndex, D3D12_GPU_DESCRIPTOR_HANDLE handle, const UINT & frameIndex)
{
	if (m_descHeaps.size() > 1)
	{
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_descHeaps[frameIndex].Get() };
		m_computeCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		m_computeCommandList->SetComputeRootDescriptorTable(rootIndex, m_descHeaps[frameIndex]->GetGPUDescriptorHandleForHeapStart());
	}
	else
	{
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_descHeaps[0].Get() };
		m_computeCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		m_computeCommandList->SetComputeRootDescriptorTable(rootIndex, handle);
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUIncrementHandle(const INT & resourceIndex)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descHeaps[0]->GetCPUDescriptorHandleForHeapStart(), resourceIndex, m_handleIncrementSize);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUIncrementHandle(const INT & resourceIndex)
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descHeaps[0]->GetGPUDescriptorHandleForHeapStart(), resourceIndex, m_handleIncrementSize);
}

std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> DescriptorHeap::GetCPUIncrementHandleForMultipleHeaps(const INT & resourceIndex)
{
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handlers;

	for (UINT i = 0; i < m_descHeaps.size(); ++i)
		handlers.push_back(CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descHeaps[i]->GetCPUDescriptorHandleForHeapStart(), resourceIndex, m_handleIncrementSize));
	
	return handlers;
}
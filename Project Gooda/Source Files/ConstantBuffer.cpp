#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer(const void* bufferData, int size, ID3D12Device* device, ID3D12GraphicsCommandList* commandList) : 
	m_device(device), m_commandList(commandList), m_bufferData(bufferData)
{
	m_size = size;

	CreateUploadHeap();
	CreateConstantBufferViewDesc();
}

ConstantBuffer::~ConstantBuffer()
{
}

void ConstantBuffer::UpdateConstantBufferData(int currentFrame)
{
	//Copy the ConstantBuffer instance to the mapped constant buffer resource
	memcpy(m_constantBufferGPUAddress[currentFrame], m_bufferData, m_size);
}

int ConstantBuffer::GetConstantBufferSize()
{
	return (sizeof(m_size) + 255) & ~255;
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetBufferLocation(int currentFrame)
{
	return m_constantBufferUploadHeap[currentFrame]->GetGPUVirtualAddress();
}

D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBuffer::GetConstantBufferViewDesc(int frameIndex)
{
	return m_cbvDesc[frameIndex];
}

void ConstantBuffer::CreateUploadHeap()
{
	//Create an upload heap for each frame buffer
	for (int i = 0; i < frameBufferCount; ++i)
	{
		assert(!m_device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(1024 * 256), D3D12_RESOURCE_STATE_GENERIC_READ,
			NULL, __uuidof(ID3D12Resource), (void**)&m_constantBufferUploadHeap[i]));

		//Map the constant buffer data to the upload heap
		//and do the initial copy
		CD3DX12_RANGE readRange(0, 0);
		assert(!m_constantBufferUploadHeap[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_constantBufferGPUAddress[i])));
		memcpy(m_constantBufferGPUAddress[i], &m_bufferData, m_size);
	}
}

void ConstantBuffer::CreateConstantBufferViewDesc()
{
	//Create a constant buffer view description
	for (int i = 0; i < frameBufferCount; ++i)
	{
		m_cbvDesc[i].BufferLocation = GetBufferLocation(i);
		m_cbvDesc[i].SizeInBytes = GetConstantBufferSize();
	}
}

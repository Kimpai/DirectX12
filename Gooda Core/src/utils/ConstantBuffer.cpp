#include "ConstantBuffer.h"

namespace GoodaCore
{
	ConstantBuffer::ConstantBuffer(const void* bufferData, UINT size) : m_bufferData(bufferData), m_size(size)
	{
		CreateUploadHeap();
		CreateConstantBufferViewDesc();
	}

	ConstantBuffer::~ConstantBuffer()
	{
		for (int i = 0; i < frameBufferCount; i++)
		{
			m_constantBufferUploadHeap[i]->Release();
		}
	}

	void ConstantBuffer::UpdateConstantBufferData(UINT currentFrame)
	{
		//Copy the ConstantBuffer instance to the mapped constant buffer resource
		memcpy(m_constantBufferGPUAddress[currentFrame], m_bufferData, m_size);
	}

	int ConstantBuffer::GetConstantBufferSize()
	{
		return (sizeof(m_size) + 255) & ~255;
	}

	D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetBufferLocation(UINT currentFrame)
	{
		return m_constantBufferUploadHeap[currentFrame]->GetGPUVirtualAddress();
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC* ConstantBuffer::GetConstantBufferViewDesc(UINT frameIndex)
	{
		return &m_cbvDesc[frameIndex];
	}

	void ConstantBuffer::CreateUploadHeap()
	{
		D3D12_HEAP_PROPERTIES heapProp = {};
		ZeroMemory(&heapProp, sizeof(D3D12_HEAP_PROPERTIES));
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resourceDesc = {};
		ZeroMemory(&resourceDesc, sizeof(D3D12_RESOURCE_DESC));
		resourceDesc.Width = 1024 * 64;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.MipLevels = 1;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		D3D12_RANGE readRange = {};
		ZeroMemory(&readRange, sizeof(D3D12_RANGE));
		readRange.Begin = 0;
		readRange.End = 0;

		//Create an upload heap for each frame buffer
		for (int i = 0; i < frameBufferCount; ++i)
		{
			assert(!Direct3D12::Instance()->GetDevice()->CreateCommittedResource(&heapProp,
				D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
				NULL, __uuidof(ID3D12Resource), (void**)& m_constantBufferUploadHeap[i]));

			//Map the constant buffer data to the upload heap
			//and do the initial copy
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
}
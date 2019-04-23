#include "IndexBuffer.h"

namespace GoodaCore
{
	IndexBuffer::IndexBuffer(const void* bufferData, int size, int offset) : m_bufferData(bufferData)
	{
		m_size = size;
		m_offset = offset;

		//Create the default heap
		CreateDefaultHeap();

		//Create the upload heap
		CreateUploadHeap();

		//Copy the index data from upload to default heap
		CopyIndexBufferData();
	}

	void IndexBuffer::SetIndexBuffer()
	{
		//Set the index buffer using the index buffer view
		Direct3D12::Instance()->GetCommandList()->IASetIndexBuffer(&m_bufferView);
	}

	void IndexBuffer::CreateDefaultHeap()
	{
		D3D12_HEAP_PROPERTIES heapProp = {};
		ZeroMemory(&heapProp, sizeof(D3D12_HEAP_PROPERTIES));
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC resourceDesc = {};
		ZeroMemory(&resourceDesc, sizeof(D3D12_RESOURCE_DESC));
		resourceDesc.Width = m_size;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.MipLevels = 1;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		//Create default heap to hold index buffer
		assert(!Direct3D12::Instance()->GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)& m_defaultHeap));

		//Give the resource a name for debugging purposes
		m_defaultHeap->SetName(L"Index Buffer Resource Heap");
	}

	void IndexBuffer::CreateUploadHeap()
	{
		D3D12_HEAP_PROPERTIES heapProp = {};
		ZeroMemory(&heapProp, sizeof(D3D12_HEAP_PROPERTIES));
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resourceDesc = {};
		ZeroMemory(&resourceDesc, sizeof(D3D12_RESOURCE_DESC));
		resourceDesc.Width = m_size;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.MipLevels = 1;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		//Create upload heap to upload index buffer
		assert(!Direct3D12::Instance()->GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)& m_uploadHeap));

		//Give the index buffer upload heap a name for debugging purposes
		m_uploadHeap->SetName(L"Index Buffer Upload Resource Heap");
	}

	void IndexBuffer::CopyIndexBufferData()
	{
		D3D12_SUBRESOURCE_DATA indexData = {};

		//Store index buffer in upload heap
		indexData.pData = m_bufferData;
		indexData.RowPitch = m_size;
		indexData.SlicePitch = m_size;

		//Create command to copy data from upload heap to default heap
		UpdateSubresources(Direct3D12::Instance()->GetCommandList(), m_defaultHeap.Get(), m_uploadHeap.Get(), 0, 0, 1, &indexData);

		//Transition the index buffer data form copy destination state to index buffer state
		D3D12_RESOURCE_BARRIER barrier = {};
		ZeroMemory(&barrier, sizeof(D3D12_RESOURCE_BARRIER));
		barrier.Transition.pResource = m_defaultHeap.Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

		Direct3D12::Instance()->GetCommandList()->ResourceBarrier(1, &barrier);

		//Create index buffer view for the triangle
		m_bufferView.BufferLocation = m_defaultHeap->GetGPUVirtualAddress();
		m_bufferView.Format = DXGI_FORMAT_R32_UINT;
		m_bufferView.SizeInBytes = m_size;
	}
}
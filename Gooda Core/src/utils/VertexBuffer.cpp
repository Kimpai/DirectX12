#include "VertexBuffer.h"

namespace GoodaCore
{
	VertexBuffer::VertexBuffer(const void* bufferData, int size, int offset) : m_bufferData(bufferData)
	{
		m_size = size;
		m_offset = offset;

		//Create the default heap
		CreateDefaultHeap();

		//Create the upload heap
		CreateUploadHeap();

		//Copy the vertex data from upload to default heap
		CopyVertexBufferData();
	}

	void VertexBuffer::SetVertexBuffer()
	{
		//Set the vertex buffer using the vertex buffer view
		Direct3D12::Instance()->GetCommandList()->IASetVertexBuffers(0, 1, &m_bufferView);
	}

	void VertexBuffer::CreateDefaultHeap()
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

		//Create default heap
		//default heap is memory on the GPU. Only GPU have access to this memory
		//To get data into this heap upload the data using an upload heap
		Direct3D12::Instance()->GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)m_defaultHeap.GetAddressOf());

		//Give resource heaps a name so when debugging with the graphics debugger
		//we know what we are looking at
		m_defaultHeap->SetName(L"Vertex Buffer Deafult Heap");
	}

	void VertexBuffer::CreateUploadHeap()
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

		//Create the upload heap
		//Upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
		Direct3D12::Instance()->GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)m_uploadHeap.GetAddressOf());

		//Give vertex buffer upload heap a name for debugging purposes
		m_uploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");
	}

	void VertexBuffer::CopyVertexBufferData()
	{
		D3D12_SUBRESOURCE_DATA vertexData = {};
		ZeroMemory(&vertexData, sizeof(D3D12_SUBRESOURCE_DATA));

		//Store vertex buffer in upload heap
		vertexData.pData = m_bufferData;
		vertexData.RowPitch = m_size;
		vertexData.SlicePitch = m_size;

		//Create a command with the command list to copy data from the upload heap to the default heap
		UpdateSubresources(Direct3D12::Instance()->GetCommandList(), m_defaultHeap.Get(), m_uploadHeap.Get(), 0, 0, 1, &vertexData);

		//Transition the vertex buffer data from copy destination to vertex buffer state
		D3D12_RESOURCE_BARRIER barrier = {};
		ZeroMemory(&barrier, sizeof(D3D12_RESOURCE_BARRIER));
		barrier.Transition.pResource = m_defaultHeap.Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

		Direct3D12::Instance()->GetCommandList()->ResourceBarrier(1, &barrier);

		//Create a vertex buffer view for the triangle.
		//Get the GPU memory address to the vertex pointer using GetGPUVirtualAddress()
		m_bufferView.BufferLocation = m_defaultHeap->GetGPUVirtualAddress();
		m_bufferView.StrideInBytes = m_offset;
		m_bufferView.SizeInBytes = m_size;
	}
}
#include "VertexBuffer.h"

namespace GoodaCore
{
	VertexBuffer::VertexBuffer(const void* bufferData, int size, int offset, ID3D12Device* device, ID3D12GraphicsCommandList* commandList) :
		m_Device(device), m_CommandList(commandList), m_bufferData(bufferData)
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

	VertexBuffer::~VertexBuffer()
	{

	}

	void VertexBuffer::SetVertexBuffer()
	{
		//Set the vertex buffer using the vertex buffer view
		m_CommandList->IASetVertexBuffers(0, 1, &m_bufferView);
	}

	void VertexBuffer::CreateDefaultHeap()
	{
		//Create default heap
		//default heap is memory on the GPU. Only GPU have access to this memory
		//To get data into this heap upload the data using an upload heap
		assert(!m_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_size), D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)& m_DefaultHeap));

		//Give resource heaps a name so when debugging with the graphics debugger
		//we know what we are looking at
		m_DefaultHeap->SetName(L"Vertex Buffer Deafult Heap");
	}

	void VertexBuffer::CreateUploadHeap()
	{
		//Create the upload heap
		//Upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
		assert(!m_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_size), D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)& m_UploadHeap));

		//Give vertex buffer upload heap a name for debugging purposes
		m_UploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");
	}

	void VertexBuffer::CopyVertexBufferData()
	{
		D3D12_SUBRESOURCE_DATA vertexData = {};

		//Store vertex buffer in upload heap
		vertexData.pData = m_bufferData;
		vertexData.RowPitch = m_size;
		vertexData.SlicePitch = m_size;

		//Create a command with the command list to copy data from the upload heap to the default heap
		UpdateSubresources(m_CommandList, m_DefaultHeap.Get(), m_UploadHeap.Get(), 0, 0, 1, &vertexData);

		//Transition the vertex buffer data from copy destination to vertex buffer state
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DefaultHeap.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		//Create a vertex buffer view for the triangle.
		//Get the GPU memory address to the vertex pointer using GetGPUVirtualAddress()
		m_bufferView.BufferLocation = m_DefaultHeap->GetGPUVirtualAddress();
		m_bufferView.StrideInBytes = m_offset;
		m_bufferView.SizeInBytes = m_size;
	}
}
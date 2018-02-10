#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const void* bufferData, int size, int offset, ID3D12Device* device, ID3D12GraphicsCommandList* commandList) :
	m_Device(device), m_CommandList(commandList), m_bufferData(bufferData)
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

IndexBuffer::~IndexBuffer()
{
}

void IndexBuffer::SetIndexBuffer()
{
	//Set the index buffer using the index buffer view
	m_CommandList->IASetIndexBuffer(&m_bufferView);
}

void IndexBuffer::CreateDefaultHeap()
{
	//Create default heap to hold index buffer
	assert(!m_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(m_size), D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)&m_DefaultHeap));

	//Give the resource a name for debugging purposes
	m_DefaultHeap->SetName(L"Index Buffer Resource Heap");
}

void IndexBuffer::CreateUploadHeap()
{
	//Create upload heap to upload index buffer
	assert(!m_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(m_size), D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)&m_UploadHeap));

	//Give the index buffer upload heap a name for debugging purposes
	m_UploadHeap->SetName(L"Index Buffer Upload Resource Heap");
}

void IndexBuffer::CopyIndexBufferData()
{
	D3D12_SUBRESOURCE_DATA indexData = {};

	//Store index buffer in upload heap
	indexData.pData = m_bufferData;
	indexData.RowPitch = m_size;
	indexData.SlicePitch = m_size;

	//Create command to copy data from upload heap to default heap
	UpdateSubresources(m_CommandList, m_DefaultHeap.Get(), m_UploadHeap.Get(), 0, 0, 1, &indexData);

	//Transition the index buffer data form copy destination state to index buffer state
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DefaultHeap.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	//Create index buffer view for the triangle
	m_bufferView.BufferLocation = m_DefaultHeap->GetGPUVirtualAddress();
	m_bufferView.Format = DXGI_FORMAT_R32_UINT;
	m_bufferView.SizeInBytes = m_size;
}

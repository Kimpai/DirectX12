#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = nullptr;
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	bool result;

	result = InitializeBuffers(device, commandList);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	ShutdownBuffers();
}

void ModelClass::Render(ID3D12GraphicsCommandList* commandList)
{
	RenderBuffers(commandList);
}

bool ModelClass::InitializeBuffers(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	HRESULT result;
	VertexType vertices[3];
	int vertexBufferSize;
	ID3D12Resource* vertexBufferUploadHeap;
	D3D12_SUBRESOURCE_DATA vertexData = {};

	//Create vertex buffer
	vertices[0].position = { 0.0f, 0.5f, 0.5f };
	vertices[1].position = { 0.5f, -0.5f, 0.5f };
	vertices[2].position = { -0.5f, -0.5f, 0.5f };

	vertexBufferSize = sizeof(vertices);

	//Create default heap
	//default heap is memory on the GPU. Only GPU have access to this memory
	//To get data into this heap upload the data using an upload heap
	result = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize), D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)&m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Give resource heaps a name so when debugging with the graphics debugger
	//we know what we are looking at
	m_vertexBuffer->SetName(L"Vertex Buffer Resource Heap");

	//Create the upload heap
	//Upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
	result = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)&vertexBufferUploadHeap);
	if (FAILED(result))
	{
		return false;
	}

	vertexBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

	//Store vertex buffer in upload heap
	vertexData.pData = reinterpret_cast<BYTE*>(vertices);
	vertexData.RowPitch = vertexBufferSize;
	vertexData.SlicePitch = vertexBufferSize;

	//Create a command with the command list to copy data from the upload heap to the default heap
	UpdateSubresources(commandList, m_vertexBuffer, vertexBufferUploadHeap, 0, 0, 1, &vertexData);

	//Transition the vertex buffer data from copy destination to vertex buffer state
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	//Create a vertex buffer view for the triangle.
	//Get the GPU memory address to the vertex pointer using GetGPUVirtualAddress()
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = sizeof(VertexType);
	m_vertexBufferView.SizeInBytes = vertexBufferSize;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	//Release the vertex buffer
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

void ModelClass::RenderBuffers(ID3D12GraphicsCommandList* commandList)
{
	//Set the primitive topology
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//Set the vertex buffer using the vertex buffer view
	commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
}

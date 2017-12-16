#include "cubeclass.h"

Cube::Cube()
{
	m_vertexBuffer = nullptr;
	m_vertexBufferUploadHeap = nullptr;
	m_indexBuffer = nullptr;
	m_indexBufferUploadHeap = nullptr;
	m_constantBufferDescHeap[0] = nullptr;
	m_constantBufferDescHeap[1] = nullptr;
	m_constantBufferDescHeap[2] = nullptr;
	m_constantBufferUploadHeap[0] = nullptr;
	m_constantBufferUploadHeap[1] = nullptr;
	m_constantBufferUploadHeap[2] = nullptr;
	m_constantBufferGPUAddress[0] = nullptr;
	m_constantBufferGPUAddress[1] = nullptr;
	m_constantBufferGPUAddress[2] = nullptr;
	m_cubeIndices = 0;
}

Cube::Cube(const Cube& other)
{

}

Cube::~Cube()
{
	//Release the vertex buffer
	if (m_vertexBuffer)
	{
		m_vertexBuffer = nullptr;
	}

	//Release the index buffer
	if (m_indexBuffer)
	{
		m_indexBuffer = nullptr;
	}

	//Release the vertex buffer upload heap
	if (m_vertexBufferUploadHeap)
	{
		m_vertexBufferUploadHeap = nullptr;
	}

	//Release the index buffer upload heap
	if (m_indexBufferUploadHeap)
	{
		m_indexBufferUploadHeap = nullptr;
	}

	//Release the constant buffer descriptor heap
	for (int i = 0; i < frameBufferCount; i++)
	{
		if (m_constantBufferDescHeap[i])
			m_constantBufferDescHeap[i] = nullptr;
	}

	//Release the constant buffer upload heap
	for (int i = 0; i < frameBufferCount; i++)
	{
		if (m_constantBufferUploadHeap[i])
			m_constantBufferUploadHeap[i] = nullptr;
	}

	//Release the constant buffer GPU Address
	for (int i = 0; i < frameBufferCount; i++)
	{
		if (m_constantBufferGPUAddress[i])
			m_constantBufferGPUAddress[i] = nullptr;
	}
}

bool Cube::InitializeBuffers(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, XMFLOAT4 origin)
{
	HRESULT result;
	int vertexBufferSize;
	int indexBufferSize;
	D3D12_SUBRESOURCE_DATA vertexData = {};
	D3D12_SUBRESOURCE_DATA indexData = {};

	//Create vertex buffer
	VertexPositionColor vertices[] =
	{
		//Front face
		{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		//Rigth face
		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		//Left face
		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		//Back face
		{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		//Top face
		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		//Bottom face
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }
	};

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
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)&m_vertexBufferUploadHeap);
	if (FAILED(result))
	{
		return false;
	}

	//Give vertex buffer upload heap a name for debugging purposes
	m_vertexBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

	//Store vertex buffer in upload heap
	vertexData.pData = reinterpret_cast<BYTE*>(vertices);
	vertexData.RowPitch = vertexBufferSize;
	vertexData.SlicePitch = vertexBufferSize;

	//Create a command with the command list to copy data from the upload heap to the default heap
	UpdateSubresources(commandList, m_vertexBuffer.Get(), m_vertexBufferUploadHeap.Get(), 0, 0, 1, &vertexData);

	//Transition the vertex buffer data from copy destination to vertex buffer state
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	//Create a vertex buffer view for the triangle.
	//Get the GPU memory address to the vertex pointer using GetGPUVirtualAddress()
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = sizeof(XMFLOAT3) + sizeof(XMFLOAT4);
	m_vertexBufferView.SizeInBytes = vertexBufferSize;

	//Create index buffer
	DWORD indices[] =
	{
		//Front face
		0, 1, 2, //First triangle
		0, 3, 1, //Second triangle

		//Rigth face
		4, 5, 6,
		4, 7, 5,

		//Left face
		8, 9, 10,
		8, 11, 9,

		//Back face
		12, 13, 14,
		12, 15, 13,

		//Top face
		16, 17, 18,
		16, 19, 17,

		//Bottom face
		20, 21, 22,
		20, 23, 21
	};

	m_cubeIndices = sizeof(indices) / sizeof(*indices);

	indexBufferSize = sizeof(indices);

	//Create default heap to hold index buffer
	result = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize), D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)&m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Give the resource a name for debugging purposes
	m_indexBuffer->SetName(L"Index Buffer Resource Heap");

	//Create upload heap to upload index buffer
	result = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)&m_indexBufferUploadHeap);
	if (FAILED(result))
	{
		return false;
	}

	//Give the index buffer upload heap a name for debugging purposes
	m_indexBufferUploadHeap->SetName(L"Index Buffer Upload Resource Heap");

	//Store index buffer in upload heap
	indexData.pData = reinterpret_cast<BYTE*>(indices);
	indexData.RowPitch = indexBufferSize;
	indexData.SlicePitch = indexBufferSize;

	//Create command to copy data from upload heap to default heap
	UpdateSubresources(commandList, m_indexBuffer.Get(), m_indexBufferUploadHeap.Get(), 0, 0, 1, &indexData);

	//Transition the index buffer data form copy destination state to index buffer state
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_indexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	//Create index buffer view for the triangle
	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = indexBufferSize;


	//Create one resource for each frame buffer
	//for our cube
	for (int i = 0; i < frameBufferCount; ++i)
	{
		result = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(1024 * 256), D3D12_RESOURCE_STATE_GENERIC_READ,
			NULL, __uuidof(ID3D12Resource), (void**)&m_constantBufferUploadHeap[i]);
		if (FAILED(result))
		{
			return false;
		}

		ZeroMemory(&m_constantBuffer, sizeof(m_constantBuffer));

		CD3DX12_RANGE readRange(0, 0);

		result = m_constantBufferUploadHeap[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_constantBufferGPUAddress[i]));

		memcpy(m_constantBufferGPUAddress[i], &m_constantBuffer, sizeof(m_constantBuffer));
	}

	return true;
}

void Cube::RenderBuffers(ID3D12GraphicsCommandList* commandList, int currentFrame)
{
	//Set the primitive topology
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set the vertex buffer using the vertex buffer view
	commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);

	//Set the index buffer using the index buffer view
	commandList->IASetIndexBuffer(&m_indexBufferView);

	//Set constant buffer
	commandList->SetGraphicsRootConstantBufferView(0, m_constantBufferUploadHeap[currentFrame]->GetGPUVirtualAddress());

	//Draw
	commandList->DrawIndexedInstanced(m_cubeIndices, 1, 0, 0, 0);
}

void Cube::ShutdownBuffers()
{
	if (m_vertexBuffer)
		m_vertexBuffer = nullptr;
	
	if (m_vertexBufferUploadHeap)
		m_vertexBufferUploadHeap = nullptr;

	if (m_indexBuffer)
		m_indexBuffer = nullptr;
	
	if (m_indexBufferUploadHeap)
		m_indexBufferUploadHeap = nullptr;
	
	if (m_constantBufferDescHeap)
	{
		m_constantBufferDescHeap[0] = nullptr;
		m_constantBufferDescHeap[1] = nullptr;
		m_constantBufferDescHeap[2] = nullptr;
	}
	
	if (m_constantBufferUploadHeap)
	{
		m_constantBufferUploadHeap[0] = nullptr;
		m_constantBufferUploadHeap[1] = nullptr;
		m_constantBufferUploadHeap[2] = nullptr;
	}
	
	if (m_constantBufferGPUAddress)
	{
		m_constantBufferGPUAddress[0] = nullptr;
		m_constantBufferGPUAddress[1] = nullptr;
		m_constantBufferGPUAddress[2] = nullptr;
	}
}

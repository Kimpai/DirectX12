#include "Cube.h"

Cube::Cube(XMFLOAT3 origin)
{
	m_constantBufferDescHeap[0] = nullptr;
	m_constantBufferDescHeap[1] = nullptr;
	m_constantBufferDescHeap[2] = nullptr;
	m_constantBufferUploadHeap[0] = nullptr;
	m_constantBufferUploadHeap[1] = nullptr;
	m_constantBufferUploadHeap[2] = nullptr;
	m_constantBufferGPUAddress[0] = nullptr;
	m_constantBufferGPUAddress[1] = nullptr;
	m_constantBufferGPUAddress[2] = nullptr;
	m_Indices = 0;
	m_origin = origin;
}

Cube::~Cube()
{
	if (m_vertexBuffer)
	{
		delete m_vertexBuffer;
		m_vertexBuffer = nullptr;
	}

	if (m_indexBuffer)
	{
		delete m_indexBuffer;
		m_indexBuffer = nullptr;
	}
}

void Cube::InitializeBuffers(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	//Create vertex data
	VertexPositionColor vertices[] =
	{
		//Front face
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y + 0.5f, m_origin.z - 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y - 0.5f, m_origin.z - 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y - 0.5f, m_origin.z - 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y + 0.5f, m_origin.z - 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },

		//Rigth face
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y - 0.5f, m_origin.z - 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y + 0.5f, m_origin.z + 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y - 0.5f, m_origin.z + 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y + 0.5f, m_origin.z - 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },

		//Left face
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y + 0.5f, m_origin.z + 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y - 0.5f, m_origin.z - 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y - 0.5f, m_origin.z + 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y + 0.5f, m_origin.z - 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },

		//Back face
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y + 0.5f, m_origin.z + 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y - 0.5f, m_origin.z + 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y - 0.5f, m_origin.z + 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y + 0.5f, m_origin.z + 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },

		//Top face
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y + 0.5f, m_origin.z + 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y + 0.5f, m_origin.z - 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y + 0.5f, m_origin.z - 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y + 0.5f, m_origin.z +  0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },

		//Bottom face
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y - 0.5f, m_origin.z + 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y - 0.5f, m_origin.z - 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y - 0.5f, m_origin.z - 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y - 0.5f, m_origin.z + 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }
	};

	//Create index data
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

	//Determine how many indices to draw
	m_Indices = sizeof(indices) / sizeof(*indices);

	//Determine the size of the vertex and index buffer
	int vertexBufferSize = sizeof(vertices);
	int indexBufferSize = sizeof(indices);

	//Create a vertex buffer
	m_vertexBuffer = new VertexBuffer(vertices, vertexBufferSize, sizeof(VertexPositionColor), device, commandList);

	//Create an index buffer
	m_indexBuffer = new IndexBuffer(indices, indexBufferSize, sizeof(DWORD), device, commandList);

	//Create one resource for each frame buffer
	//for our cube
	for (int i = 0; i < frameBufferCount; ++i)
	{
		assert(!device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(1024 * 256), D3D12_RESOURCE_STATE_GENERIC_READ,
			NULL, __uuidof(ID3D12Resource), (void**)&m_constantBufferUploadHeap[i]));

		ZeroMemory(&m_constantBuffer, sizeof(m_constantBuffer));

		CD3DX12_RANGE readRange(0, 0);
		assert(!m_constantBufferUploadHeap[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_constantBufferGPUAddress[i])));
		memcpy(m_constantBufferGPUAddress[i], &m_constantBuffer, sizeof(m_constantBuffer));
	}
}

void Cube::RenderBuffers(ID3D12GraphicsCommandList* commandList, int currentFrame)
{
	//Set the primitive topology
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set the vertex buffer
	m_vertexBuffer->SetVertexBuffer();

	//Set the index buffer
	m_indexBuffer->SetIndexBuffer();

	//Set constant buffer
	commandList->SetGraphicsRootConstantBufferView(0, m_constantBufferUploadHeap[currentFrame]->GetGPUVirtualAddress());

	//Draw
	commandList->DrawIndexedInstanced(m_Indices, 1, 0, 0, 0);
}



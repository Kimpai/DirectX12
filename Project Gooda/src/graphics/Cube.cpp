#include "Cube.h"

Cube::Cube(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, XMFLOAT3 origin, XMFLOAT4 color) : Model(device, commandList, origin),
			m_origin(origin), m_color(color), m_indices(0)
{
	InitializeBuffers(device, commandList);
}

Cube::~Cube()
{
	if (m_vertexBuffer)
		m_vertexBuffer->Release();

	if (m_indexBuffer)
		m_indexBuffer->Release();

	if (m_constantBuffer)
		m_constantBuffer->Release();
}

ConstantBuffer* Cube::GetConstantBuffer()
{
	return m_constantBuffer;
}

XMFLOAT3 Cube::GetPosition()
{
	return m_origin;
}

void Cube::InitializeBuffers(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	//Create vertex data
	VertexPositionNormalColor vertices[] =
	{
		//Front face
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y + 0.5f, m_origin.z - 0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f), m_color },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y - 0.5f, m_origin.z - 0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f), m_color },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y - 0.5f, m_origin.z - 0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f), m_color },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y + 0.5f, m_origin.z - 0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f), m_color },

		//Rigth face
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y - 0.5f, m_origin.z - 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), m_color },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y + 0.5f, m_origin.z + 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), m_color },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y - 0.5f, m_origin.z + 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), m_color },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y + 0.5f, m_origin.z - 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), m_color },

		//Left face
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y + 0.5f, m_origin.z + 0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), m_color },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y - 0.5f, m_origin.z - 0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), m_color },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y - 0.5f, m_origin.z + 0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), m_color },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y + 0.5f, m_origin.z - 0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), m_color },

		//Back face
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y + 0.5f, m_origin.z + 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), m_color },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y - 0.5f, m_origin.z + 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), m_color },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y - 0.5f, m_origin.z + 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), m_color },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y + 0.5f, m_origin.z + 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), m_color },

		//Top face
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y + 0.5f, m_origin.z - 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), m_color },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y + 0.5f, m_origin.z + 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), m_color },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y + 0.5f, m_origin.z - 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), m_color },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y + 0.5f, m_origin.z + 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), m_color },

		//Bottom face
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y - 0.5f, m_origin.z + 0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f), m_color },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y - 0.5f, m_origin.z - 0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f), m_color },
		{ XMFLOAT3(m_origin.x + 0.5f, m_origin.y - 0.5f, m_origin.z - 0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f), m_color },
		{ XMFLOAT3(m_origin.x - 0.5f, m_origin.y - 0.5f, m_origin.z + 0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f), m_color }
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
	m_indices = sizeof(indices) / sizeof(*indices);

	//Determine the size of the vertex and index buffer
	int vertexBufferSize = _countof(vertices) * sizeof(VertexPositionNormalColor);
	int indexBufferSize = _countof(indices) * sizeof(DWORD);

	//Create a vertex buffer
	m_vertexBuffer = new VertexBuffer(vertices, vertexBufferSize, sizeof(VertexPositionNormalColor), device, commandList);

	//Create an index buffer
	m_indexBuffer = new IndexBuffer(indices, indexBufferSize, sizeof(DWORD), device, commandList);

	//Create a constant buffer for the world, view and projection matrices
	m_constantBuffer = new ConstantBuffer(&m_constantBufferData, sizeof(ConstantBufferData), device, commandList);
}

void Cube::Render(ID3D12GraphicsCommandList* commandList, int currentFrame, int rootIndex, CD3DX12_GPU_DESCRIPTOR_HANDLE handle)
{
	//Set the primitive topology
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set the vertex buffer
	m_vertexBuffer->SetVertexBuffer();

	//Set the index buffer
	m_indexBuffer->SetIndexBuffer();

	//Set the constant buffer
	m_constantBuffer->SetConstantBuffer(rootIndex, handle);

	//Draw
	commandList->DrawIndexedInstanced(m_indices, 1, 0, 0, 0);
}



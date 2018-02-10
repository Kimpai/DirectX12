#include "Terrain.h"

Terrain::Terrain(int height, int width)
{
	m_height = height;
	m_width = width;
}

Terrain::~Terrain()
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

void Terrain::InitializeBuffers(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	//Determine the size of the vertex and index data
	//also used to set size of the vertex and index buffer
	const int size = (m_width - 1) * (m_height - 1) * 8;

	XMFLOAT4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
	VertexPositionColor* vertices = new VertexPositionColor[size];
	unsigned int* indices = new unsigned int[size];
	
	// Initialize the index of the vertex and index arrays to zero.
	int index = 0;

	// Load the vertex array and index array with data.
	float positionX, positionZ;
	for (int j = 0; j < (m_height - 1); j++)
	{
		for (int i = 0; i < (m_width - 1); i++)
		{
			// Line 1 - Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 1 - Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 2 - Upper right
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 2 - Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 3 - Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 3 - Bottom left.
			positionX = (float)i;
			positionZ = (float)j;

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 4 - Bottom left.
			positionX = (float)i;
			positionZ = (float)j;

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 4 - Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;
		}
	}

	//Determine how many indices to draw
	m_Indices = size;

	//Determine the size of the vertex and index buffers
	int vertexBufferSize = size * 4;
	int indexBufferSize = size * 4;

	//Create a vertex buffer
	m_vertexBuffer = new VertexBuffer(vertices, vertexBufferSize, sizeof(VertexPositionColor), device, commandList);
	
	//Create an index buffer
	m_indexBuffer = new IndexBuffer(indices, indexBufferSize, sizeof(DWORD), device, commandList);

	//Create one resource for each frame buffer
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

void Terrain::RenderBuffers(ID3D12GraphicsCommandList* commandList, int currentFrame)
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

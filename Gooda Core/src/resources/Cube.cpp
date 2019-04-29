#include "Cube.h"

namespace GoodaCore
{
	Cube::Cube(XMFLOAT3 origin, XMFLOAT4 color) : Model(origin),
		m_origin(origin), m_color(color), m_indices(0)
	{
		InitializeBuffers();

		ShaderManager::Instance()->CreateDescriptor(ObjectType::MODEL, m_constantBuffer->GetConstantBufferViewDesc(0));
	}

	Cube::~Cube()
	{
		m_vertexBuffer->Release();
		m_indexBuffer->Release();
		m_constantBuffer->Release();
	}

	void Cube::InitializeBuffers()
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
		UINT indices[] =
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
		m_indices = sizeof(indices) / sizeof(UINT);

		//Determine the size of the vertex and index buffer
		UINT vertexBufferSize = _countof(vertices) * sizeof(VertexPositionNormalColor);
		UINT indexBufferSize = _countof(indices) * sizeof(UINT);

		//Create a vertex buffer
		m_vertexBuffer = new VertexBuffer(vertices, vertexBufferSize, sizeof(VertexPositionNormalColor));

		//Create an index buffer
		m_indexBuffer = new IndexBuffer(indices, indexBufferSize, sizeof(UINT));

		//Create a constant buffer for the world, projection matrices
		m_constantBuffer = new ConstantBuffer(&m_constantBufferData, sizeof(ConstantBufferData));
	}

	void Cube::Draw()
	{
		//Set the primitive topology
		Direct3D12::Instance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Set the vertex buffer
		m_vertexBuffer->SetVertexBuffer();

		//Set the index buffer
		m_indexBuffer->SetIndexBuffer();

		//Draw
		Direct3D12::Instance()->GetCommandList()->DrawIndexedInstanced(m_indices, 1, 0, 0, 0);
	}
}
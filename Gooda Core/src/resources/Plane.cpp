#include "Plane.h"

namespace GoodaCore
{
	Plane::Plane(UINT width, UINT height, XMFLOAT3 origin, XMFLOAT4 color) : Model(origin),
		m_origin(origin), m_color(color), m_height(height), m_width(width), m_indices(0)
	{
		InitializeBuffers();

		ShaderManager::Instance()->CreateDescriptor(ObjectType::MODEL, m_constantBuffer->GetConstantBufferViewDesc(0));
	}

	Plane::~Plane()
	{
		m_vertexBuffer->Release();
		m_indexBuffer->Release();
		m_constantBuffer->Release();
	}

	void Plane::InitializeBuffers()
	{
		//Create vertex data
		VertexPositionNormalColor vertices[] =
		{
			{ XMFLOAT3(m_origin.x - m_width, m_origin.y, m_origin.z - m_height), XMFLOAT3(0.0f, 1.0f, 0.0f), m_color },
			{ XMFLOAT3(m_origin.x + m_width, m_origin.y, m_origin.z + m_height), XMFLOAT3(0.0f, 1.0f, 0.0f), m_color },
			{ XMFLOAT3(m_origin.x + m_width, m_origin.y, m_origin.z - m_height), XMFLOAT3(0.0f, 1.0f, 0.0f), m_color },
			{ XMFLOAT3(m_origin.x - m_width, m_origin.y, m_origin.z + m_height), XMFLOAT3(0.0f, 1.0f, 0.0f), m_color },
		};

		UINT indices[] =
		{
			//Top face
			0, 1, 2,
			0, 3, 1,
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

	void Plane::Draw()
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
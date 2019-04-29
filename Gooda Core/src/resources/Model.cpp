#include "Model.h"

namespace GoodaCore
{
	Model::Model(XMFLOAT3 origin) : m_origin(origin)
	{
		BuildWorlViewProjectionMatrix(m_origin);
	}

	Model::~Model()
	{

	}

	void Model::UpdateMatrices()
	{
		//Transpose the matrices before sending them to the GPU
		XMMATRIX transposed = XMMatrixTranspose(XMLoadFloat4x4(&m_projectionMatrix));
		XMStoreFloat4x4(&m_projectionMatrix, transposed);

		transposed = XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix));
		XMStoreFloat4x4(&m_worldMatrix, transposed);

		//Fill the constant buffer with the matrices
		m_constantBufferData.worldMatrix = m_worldMatrix;
		m_constantBufferData.projectionMatrix = m_projectionMatrix;
	}

	void Model::BuildWorlViewProjectionMatrix(XMFLOAT3 origin)
	{
		//Build projection matrix
		XMMATRIX matrix = XMMatrixPerspectiveFovLH(45.0f * (3.14f / 180.0f), (float)m_screenWidth / (float)m_screenHeight,
			m_screenNear, m_screenDepth);
		XMStoreFloat4x4(&m_projectionMatrix, matrix);

		//Build world matrix
		matrix = XMMatrixTranslationFromVector(XMLoadFloat3(&origin));
		XMStoreFloat4x4(&m_worldMatrix, matrix);
	}

	bool Model::Frame()
	{
		UINT frameIndex = Direct3D12::Instance()->GetCurrentFrame();

		//Update the matrices before copying them to the GPU
		UpdateMatrices();

		//Update the constant buffer data
		m_constantBuffer->UpdateConstantBufferData(frameIndex);

		return true;
	}
}
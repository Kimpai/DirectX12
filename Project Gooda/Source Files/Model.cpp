#include "Model.h"

Model::Model(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, XMFLOAT3 origin)
{
	BuildWorlViewProjectionMatrix(origin);
}

Model::~Model()
{

}

void Model::UpdateMatrices(XMMATRIX viewMatrix)
{
	//Get current view matrix from camera
	XMStoreFloat4x4(&m_viewMatrix, viewMatrix);

	//Transpose the matrices before sending them to the GPU
	XMMATRIX transposed = XMMatrixTranspose(XMLoadFloat4x4(&m_projectionMatrix));
	XMStoreFloat4x4(&m_projectionMatrix, transposed);

	transposed = XMMatrixTranspose(XMLoadFloat4x4(&m_viewMatrix));
	XMStoreFloat4x4(&m_viewMatrix, transposed);

	transposed = XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix));
	XMStoreFloat4x4(&m_worldMatrix, transposed);

	//Fill the constant buffer with the matrices
	m_constantBufferData.worldMatrix = m_worldMatrix;
	m_constantBufferData.viewMatrix = m_viewMatrix;
	m_constantBufferData.projectionMatrix = m_projectionMatrix;
}

void Model::BuildWorlViewProjectionMatrix(XMFLOAT3 origin)
{
	//Build projection matrix
	XMMATRIX matrix = XMMatrixPerspectiveFovLH(45.0f*(3.14f/180.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
		SCREEN_NEAR, SCREEN_DEPTH);
	XMStoreFloat4x4(&m_projectionMatrix, matrix);

	//Build view matrix
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());

	//Build world matrix
	matrix = XMMatrixTranslationFromVector(XMLoadFloat3(&origin));
	XMStoreFloat4x4(&m_worldMatrix, matrix);
}

void Model::Frame(int currentFrame, XMMATRIX viewMatrix)
{
	//Update the matrices before copying them to the GPU
	UpdateMatrices(viewMatrix);

	//Update the constant buffer data
	m_constantBuffer->UpdateConstantBufferData(currentFrame);
}

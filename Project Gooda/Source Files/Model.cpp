#include "Model.h"

Model::Model()
{

}

Model::~Model()
{

}

void Model::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, XMMATRIX viewMatrix, XMFLOAT4 origin)
{
	InitializeBuffers(device, commandList);

	BuildWorlViewProjectionMatrix(viewMatrix, origin);
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

	transposed = XMMatrixTranspose(XMLoadFloat4x4(&m_rotationMatrix));
	XMStoreFloat4x4(&m_rotationMatrix, transposed);

	transposed = XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix));
	XMStoreFloat4x4(&m_worldMatrix, transposed);

	//Fill the constant buffer with the matrices
	m_constantBufferData.worldMatrix = m_worldMatrix;
	m_constantBufferData.viewMatrix = m_viewMatrix;
	m_constantBufferData.projectionMatrix = m_projectionMatrix;
	m_constantBufferData.rotationMatrix = m_rotationMatrix;
}

void Model::BuildWorlViewProjectionMatrix(XMMATRIX viewMatrix, XMFLOAT4 origin)
{
	//Build projection matrix
	XMMATRIX matrix = XMMatrixPerspectiveFovLH(45.0f*(3.14f/180.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
		SCREEN_NEAR, SCREEN_DEPTH);
	XMStoreFloat4x4(&m_projectionMatrix, matrix);

	//Build view matrix
	XMStoreFloat4x4(&m_viewMatrix, viewMatrix);

	//Build rotation matrix
	XMStoreFloat4x4(&m_rotationMatrix, XMMatrixIdentity());

	//Build world matrix
	matrix = XMMatrixTranslationFromVector(XMLoadFloat4(&origin));
	XMStoreFloat4x4(&m_worldMatrix, matrix);
}

void Model::Frame(int currentFrame, XMMATRIX viewMatrix)
{
	//Update the matrices before copying them to the GPU
	UpdateMatrices(viewMatrix);

	//Update the constant buffer data
	m_constantBuffer->UpdateConstantBufferData(currentFrame);
}

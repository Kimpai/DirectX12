#include "modelclass.h"

Model::Model()
{

}

Model::Model(const Model& other)
{
}

Model::~Model()
{
	
}

bool Model::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Camera* camera, int width, int height, float nearplane, float farplane, XMFLOAT4 origin)
{
	bool result;

	result = InitializeBuffers(device, commandList, origin);
	if (!result)
	{
		return false;
	}

	BuildWorlViewProjectionMatrix(camera, width, height, nearplane, farplane, origin);

	return true;
}

void Model::Render(ID3D12GraphicsCommandList* commandList, int currentFrame)
{
	RenderBuffers(commandList, currentFrame);
}

void Model::Shutdown()
{
	ShutdownBuffers();
}

void Model::BuildWorlViewProjectionMatrix(Camera* camera, int width, int height, float nearPlane, float farPlane, XMFLOAT4 origin)
{
	//Build projection matrix
	XMMATRIX matrix = XMMatrixPerspectiveFovLH(45.0f*(3.14f/180.0f), (float)width / (float)height,
		nearPlane, farPlane);
	XMStoreFloat4x4(&m_constantBuffer.projectionMatrix, matrix);

	//Build view matrix
	XMVECTOR pos = XMLoadFloat4(&XMFLOAT4(camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z, 1.0f));
	XMVECTOR lookAt = XMLoadFloat4(&XMFLOAT4(camera->GetLookAt().x, camera->GetLookAt().y, camera->GetLookAt().z, 1.0f));
	XMVECTOR up = XMLoadFloat4(&XMFLOAT4(camera->GetUp().x, camera->GetUp().y, camera->GetUp().z, 1.0f));
	
	matrix = XMMatrixLookAtLH(pos, lookAt, up);
	XMStoreFloat4x4(&m_constantBuffer.viewMatrix, matrix);

	//Build rotation matrix
	XMStoreFloat4x4(&m_constantBuffer.rotationMatrix, XMMatrixIdentity());

	//Build world matrix
	pos = XMLoadFloat4(&origin);

	matrix = XMMatrixTranslationFromVector(pos);
	XMStoreFloat4x4(&m_constantBuffer.worldMatrix, matrix);

	return;
}

void Model::Frame(int currentFrame, Camera* camera)
{
	//Build view matrix
	XMVECTOR pos = XMLoadFloat4(&XMFLOAT4(camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z, 1.0f));
	XMVECTOR lookAt = XMLoadFloat4(&XMFLOAT4(camera->GetLookAt().x, camera->GetLookAt().y, camera->GetLookAt().z, 1.0f));
	XMVECTOR up = XMLoadFloat4(&XMFLOAT4(camera->GetUp().x, camera->GetUp().y, camera->GetUp().z, 1.0f));

	XMMATRIX matrix = XMMatrixLookAtLH(pos, lookAt, up);
	XMStoreFloat4x4(&m_constantBuffer.viewMatrix, matrix);

	//Transpose the matrices before sending them to the GPU
	XMMATRIX transposed = XMMatrixTranspose(XMLoadFloat4x4(&m_constantBuffer.projectionMatrix));
	XMStoreFloat4x4(&m_constantBuffer.projectionMatrix, transposed);

	transposed = XMMatrixTranspose(XMLoadFloat4x4(&m_constantBuffer.viewMatrix));
	XMStoreFloat4x4(&m_constantBuffer.viewMatrix, transposed);

	transposed = XMMatrixTranspose(XMLoadFloat4x4(&m_constantBuffer.rotationMatrix));
	XMStoreFloat4x4(&m_constantBuffer.rotationMatrix, transposed);

	transposed = XMMatrixTranspose(XMLoadFloat4x4(&m_constantBuffer.worldMatrix));
	XMStoreFloat4x4(&m_constantBuffer.worldMatrix, transposed);

	//Copy the ConstantBuffer instance to the mapped constant buffer resource
	memcpy(m_constantBufferGPUAddress[currentFrame], &m_constantBuffer, sizeof(m_constantBuffer));
}

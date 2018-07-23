#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, 
	XMFLOAT4 diffuseColor, XMFLOAT4 ambientColor, XMFLOAT4 specularColor, XMFLOAT3 lightDirection) : Light(device, commandList)
{
	m_constantBufferData.ambientColor = ambientColor,
	m_constantBufferData.diffuseColor = diffuseColor;
	m_constantBufferData.specularColor = specularColor;
	m_constantBufferData.lightDirection = lightDirection;

	InitializeBuffers(device, commandList);
}

DirectionalLight::~DirectionalLight()
{
	if (m_constantBuffer)
		m_constantBuffer->Release();
}

ConstantBuffer* DirectionalLight::GetConstantBuffer()
{
	return m_constantBuffer;
}

void DirectionalLight::InitializeBuffers(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	//Create a constant buffer for this directional light
	m_constantBuffer = new ConstantBuffer(&m_constantBufferData, sizeof(ConstantBufferData), device, commandList);
}

void DirectionalLight::Render(ID3D12GraphicsCommandList* commandList, int currentFrame, int rootIndex, CD3DX12_GPU_DESCRIPTOR_HANDLE handle)
{
	//Set the constant buffer
	m_constantBuffer->SetConstantBuffer(rootIndex, handle);
}

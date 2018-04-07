#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(XMFLOAT4 diffuseColor, XMFLOAT3 lightDirection)
{
	m_constantBufferData.diffuseColor = diffuseColor;
	m_constantBufferData.lightDirection = lightDirection;
}

DirectionalLight::~DirectionalLight()
{
	if (m_constantBuffer)
	{
		delete m_constantBuffer;
		m_constantBuffer = nullptr;
	}
}

void DirectionalLight::InitializeBuffers(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	//Create a constant buffer for the world, view and projection matrices
	m_constantBuffer = new ConstantBuffer(&m_constantBufferData, sizeof(ConstantBufferData), device, commandList);
}

void DirectionalLight::Render(ID3D12GraphicsCommandList* commandList, int currentFrame)
{
	commandList->SetGraphicsRootConstantBufferView(1, m_constantBuffer->GetBufferLocation(currentFrame));
}

#include "Light.h"

Light::Light()
{
}

Light::~Light()
{
}

void Light::Frame(int currentFrame)
{
	//Update the constant buffer data
	m_constantBuffer->UpdateConstantBufferData(currentFrame);
}

void Light::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	InitializeBuffers(device, commandList);
}

int Light::GetNumOfConstantBuffers()
{
	return m_numOfConstantBuffers;
}

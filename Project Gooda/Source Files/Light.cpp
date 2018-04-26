#include "Light.h"

Light::Light(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
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

int Light::GetNumOfConstantBuffers()
{
	return m_numOfConstantBuffers;
}

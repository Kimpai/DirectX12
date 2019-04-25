#include "DirectionalLight.h"

namespace GoodaCore
{
	DirectionalLight::DirectionalLight(XMFLOAT4 diffuseColor, XMFLOAT4 ambientColor, XMFLOAT4 specularColor, XMFLOAT3 lightDirection) : Light()
	{
		m_constantBufferData.ambientColor = ambientColor,
		m_constantBufferData.diffuseColor = diffuseColor;
		m_constantBufferData.specularColor = specularColor;
		m_constantBufferData.lightDirection = lightDirection;

		InitializeBuffers();
	}

	ConstantBuffer* DirectionalLight::GetConstantBuffer()
	{
		return m_constantBuffer;
	}

	void DirectionalLight::InitializeBuffers()
	{
		//Create a constant buffer for this directional light
		m_constantBuffer = new ConstantBuffer(&m_constantBufferData, sizeof(ConstantBufferData));

		ShaderManager::Instance()->CreateDescriptor(ObjectType::LIGHT, m_constantBuffer);

	}

	bool DirectionalLight::Init()
	{
		return true;
	}

	bool DirectionalLight::Destroy()
	{
		m_constantBuffer->Release();
		return true;
	}
}
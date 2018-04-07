#pragma once

#include <d3d12.h>
#include <DirectXMath.h>

#include "ConstantBuffer.h"

using namespace DirectX;

class Light
{
public:
	ConstantBuffer* m_constantBuffer;

	struct ConstantBufferData
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding = 0.0f;

	} m_constantBufferData;

	Light();
	virtual ~Light() = 0;

	void Initialize(ID3D12Device*, ID3D12GraphicsCommandList*);
	virtual void Frame(int);
	virtual void Render(ID3D12GraphicsCommandList*, int) = 0;

private:
	const int ConstantBufferPerObjectAlignedSize = (sizeof(m_constantBufferData) + 255) & ~255;

	virtual void InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*) = 0;
};
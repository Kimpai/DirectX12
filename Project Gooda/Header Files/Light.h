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
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding = 0.0f;

	} m_constantBufferData;

	Light(ID3D12Device*, ID3D12GraphicsCommandList*);
	virtual ~Light() = 0;

	virtual void Frame(int);
	virtual void Render(ID3D12GraphicsCommandList*, int, int, CD3DX12_GPU_DESCRIPTOR_HANDLE) = 0;
	virtual ConstantBuffer* GetConstantBuffer() = 0;
	static int GetNumOfConstantBuffers();

private:
	const static int m_numOfConstantBuffers = 1;

	virtual void InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*) = 0;
};
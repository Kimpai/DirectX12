#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <frame.h>
#include <config.h>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class Model
{
public:
	ConstantBuffer* m_constantBuffer;
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	
	struct ConstantBufferData
	{
		XMFLOAT4X4 worldMatrix;
		XMFLOAT4X4 viewMatrix;
		XMFLOAT4X4 projectionMatrix;
		XMFLOAT4X4 rotationMatrix;

	} m_constantBufferData;

	Model();
	virtual ~Model() = 0;

	void Initialize(ID3D12Device*, ID3D12GraphicsCommandList*, XMMATRIX, XMFLOAT4);
	virtual void Render(ID3D12GraphicsCommandList*, int) = 0;
	virtual void Frame(int, XMMATRIX);

private:
	XMFLOAT4X4 m_worldMatrix;
	XMFLOAT4X4 m_viewMatrix;
	XMFLOAT4X4 m_projectionMatrix;
	XMFLOAT4X4 m_rotationMatrix;

	const int ConstantBufferPerObjectAlignedSize = (sizeof(m_constantBufferData) + 255) & ~255;

	void UpdateMatrices(XMMATRIX viewMatrix);
	virtual void BuildWorlViewProjectionMatrix(XMMATRIX, XMFLOAT4);
	virtual void InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*) = 0;
};
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

	} m_constantBufferData;

	Model(ID3D12Device*, ID3D12GraphicsCommandList*, XMFLOAT3);
	virtual ~Model() = 0;

	virtual void Render(ID3D12GraphicsCommandList*, int, int, CD3DX12_GPU_DESCRIPTOR_HANDLE) = 0;
	virtual ConstantBuffer* GetConstantBuffer() = 0;
	virtual void Frame(int, XMMATRIX);
	virtual XMFLOAT3 GetPosition() { return XMFLOAT3(0.0f, 0.0f, 0.0f); };

private:
	XMFLOAT4X4 m_worldMatrix;
	XMFLOAT4X4 m_viewMatrix;
	XMFLOAT4X4 m_projectionMatrix;

	void UpdateMatrices(XMMATRIX viewMatrix);
	virtual void BuildWorlViewProjectionMatrix(XMFLOAT3);
	virtual void InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*) = 0;
};
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
	virtual void Render(ID3D12GraphicsCommandList*, int, CD3DX12_GPU_DESCRIPTOR_HANDLE) = 0;
	virtual ConstantBuffer* GetConstantBuffer() = 0;
	virtual void Frame(int, XMMATRIX);
	virtual XMFLOAT3 GetPosition() { return XMFLOAT3(0.0f, 0.0f, 0.0f); };

private:
	XMFLOAT4X4 m_worldMatrix;
	XMFLOAT4X4 m_viewMatrix;
	XMFLOAT4X4 m_projectionMatrix;
	XMFLOAT4X4 m_rotationMatrix;

	void UpdateMatrices(XMMATRIX viewMatrix);
	virtual void BuildWorlViewProjectionMatrix(XMMATRIX, XMFLOAT4);
	virtual void InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*) = 0;
};
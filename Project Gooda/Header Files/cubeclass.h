#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <VertexTypes.h>
#include "frame.h"
#include "modelclass.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class CubeClass : public ModelClass
{
public:
	CubeClass();
	CubeClass(const CubeClass&);
	~CubeClass();

private:
	struct ConstantBuffer
	{
		XMFLOAT4X4 worldMatrix;
		XMFLOAT4X4 viewMatrix;
		XMFLOAT4X4 projectionMatrix;
	};

	bool InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*);
	void RenderBuffers(ID3D12GraphicsCommandList*);
	void ShutdownBuffers();

	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	ComPtr<ID3D12Resource> m_vertexBufferUploadHeap;
	ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	ComPtr<ID3D12Resource> m_indexBufferUploadHeap;
	ComPtr<ID3D12DescriptorHeap> m_constantBufferDescHeap[frameBufferCount];
	ComPtr<ID3D12Resource> m_constantBufferUploadHeap[frameBufferCount];
	ConstantBuffer m_constantBuffer;
	UINT8* m_constantBufferGPUAddress[frameBufferCount];
};

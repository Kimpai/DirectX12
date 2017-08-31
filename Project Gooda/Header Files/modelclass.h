#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

using namespace DirectX;

class ModelClass
{
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D12Device*, ID3D12GraphicsCommandList*);
	void Shutdown();
	void Render(ID3D12GraphicsCommandList*);

private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	bool InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D12GraphicsCommandList*);

	ID3D12Resource* m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	ID3D12Resource* m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
};
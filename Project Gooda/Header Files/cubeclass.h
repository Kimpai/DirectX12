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

class Cube : public Model
{
public:
	Cube();
	Cube(const Cube&);
	~Cube();

private:
	bool InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*, XMFLOAT4);
	void RenderBuffers(ID3D12GraphicsCommandList*, int);
	void ShutdownBuffers();

	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	ComPtr<ID3D12Resource> m_vertexBufferUploadHeap;
	ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	ComPtr<ID3D12Resource> m_indexBufferUploadHeap;
	int m_cubeIndices;
};

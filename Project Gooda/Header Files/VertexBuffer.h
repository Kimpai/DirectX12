#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <assert.h>

using namespace Microsoft::WRL;

class VertexBuffer
{
public:
	VertexBuffer(const void*, int, int, ID3D12Device*, ID3D12GraphicsCommandList*);
	~VertexBuffer();

	void SetVertexBuffer();
private:
	void CreateDefaultHeap();
	void CreateUploadHeap();
	void CopyVertexBufferData();

	ID3D12Device* m_Device;
	ID3D12GraphicsCommandList* m_CommandList;
	int m_size;
	int m_offset;
	ComPtr<ID3D12Resource> m_DefaultHeap;
	ComPtr<ID3D12Resource> m_UploadHeap;
	const void* m_bufferData;
	D3D12_VERTEX_BUFFER_VIEW m_bufferView;
};
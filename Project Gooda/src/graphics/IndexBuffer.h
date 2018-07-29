#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <assert.h>
#include <Gooda.h>

using namespace Microsoft::WRL;

class IndexBuffer : public Gooda
{
public:
	IndexBuffer(const void*, int, int, ID3D12Device*, ID3D12GraphicsCommandList*);
	~IndexBuffer();

	void SetIndexBuffer();
private:
	void CreateDefaultHeap();
	void CreateUploadHeap();
	void CopyIndexBufferData();

	ID3D12Device* m_Device;
	ID3D12GraphicsCommandList* m_CommandList;
	int m_size;
	int m_offset;
	ComPtr<ID3D12Resource> m_DefaultHeap;
	ComPtr<ID3D12Resource> m_UploadHeap;
	const void* m_bufferData;
	D3D12_INDEX_BUFFER_VIEW m_bufferView;
};

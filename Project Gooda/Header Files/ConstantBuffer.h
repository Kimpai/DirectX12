#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <frame.h>
#include <DirectXMath.h>
#include <assert.h>

using namespace Microsoft::WRL;
using namespace DirectX;

class ConstantBuffer
{
public:
	ConstantBuffer(const void*, int, ID3D12Device*, ID3D12GraphicsCommandList*);
	~ConstantBuffer();

	void SetConstantBuffer(int, CD3DX12_GPU_DESCRIPTOR_HANDLE);
	void UpdateConstantBufferData(int);
	int GetConstantBufferSize();
	D3D12_GPU_VIRTUAL_ADDRESS GetBufferLocation(int);
	D3D12_CONSTANT_BUFFER_VIEW_DESC* GetConstantBufferViewDesc(int);
	D3D12_CONSTANT_BUFFER_VIEW_DESC* GetConstantBufferViewDesc();

private:
	void CreateUploadHeap();
	void CreateConstantBufferViewDesc();

	ComPtr<ID3D12Resource> m_constantBufferUploadHeap[frameBufferCount];
	UINT8* m_constantBufferGPUAddress[frameBufferCount];

	ID3D12Device* m_device;
	ID3D12GraphicsCommandList* m_commandList;
	const void* m_bufferData;
	int m_size;
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_cbvDesc[frameBufferCount];
};

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

	void UpdateConstantBufferData(int);
	D3D12_GPU_VIRTUAL_ADDRESS GetBufferLocation(int);
private:
	void CreateUploadHeap();

	ComPtr<ID3D12DescriptorHeap> m_constantBufferDescHeap[frameBufferCount];
	ComPtr<ID3D12Resource> m_constantBufferUploadHeap[frameBufferCount];
	UINT8* m_constantBufferGPUAddress[frameBufferCount];

	ID3D12Device* m_device;
	ID3D12GraphicsCommandList* m_commandList;
	const void* m_bufferData;
	int m_size;
};

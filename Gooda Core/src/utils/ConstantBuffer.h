#pragma once

#include <frame.h>
#include <DirectXMath.h>
#include <assert.h>
#include <Gooda.h>

#include "DX12/Direct3D12.h"

using namespace DirectX;

namespace GoodaCore
{
	class ConstantBuffer : public Gooda
	{
	public:
		ConstantBuffer(const void*, int);
		virtual ~ConstantBuffer() = default;

		void SetConstantBuffer(D3D12_GPU_DESCRIPTOR_HANDLE);
		void UpdateConstantBufferData(UINT);
		int GetConstantBufferSize();
		D3D12_GPU_VIRTUAL_ADDRESS GetBufferLocation(UINT);
		D3D12_CONSTANT_BUFFER_VIEW_DESC* GetConstantBufferViewDesc(UINT);

	private:
		void CreateUploadHeap();
		void CreateConstantBufferViewDesc();

		ComPtr<ID3D12Resource> m_constantBufferUploadHeap[frameBufferCount];
		UINT8* m_constantBufferGPUAddress[frameBufferCount];

		const void* m_bufferData;
		int m_size;
		D3D12_CONSTANT_BUFFER_VIEW_DESC m_cbvDesc[frameBufferCount];
	};
}
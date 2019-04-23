#pragma once

#include <assert.h>
#include <Gooda.h>

#include "DX12/Direct3D12.h"

namespace GoodaCore
{
	class IndexBuffer : public Gooda
	{
	public:
		IndexBuffer(const void*, int, int);
		virtual ~IndexBuffer() = default;

		void SetIndexBuffer();

	private:
		void CreateDefaultHeap();
		void CreateUploadHeap();
		void CopyIndexBufferData();

		int m_size;
		int m_offset;
		ComPtr<ID3D12Resource> m_defaultHeap;
		ComPtr<ID3D12Resource> m_uploadHeap;
		const void* m_bufferData;
		D3D12_INDEX_BUFFER_VIEW m_bufferView;
	};
}
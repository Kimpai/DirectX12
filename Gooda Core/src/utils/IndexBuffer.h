#pragma once

#include "DX12/Direct3D12.h"

namespace GoodaCore
{
	class IndexBuffer : public Gooda
	{
	public:
		IndexBuffer(const void*, int, int);
		virtual ~IndexBuffer();

		void SetIndexBuffer();

	private:
		void CreateDefaultHeap();
		void CreateUploadHeap();
		void CopyIndexBufferData();

		int m_size;
		int m_offset;
		ID3D12Resource* m_defaultHeap;
		ID3D12Resource* m_uploadHeap;
		const void* m_bufferData;
		D3D12_INDEX_BUFFER_VIEW m_bufferView;
	};
}
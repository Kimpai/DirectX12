#pragma once

#include "DX12/Direct3D12.h"

namespace GoodaCore
{
	class VertexBuffer : public Gooda
	{
	public:
		VertexBuffer(void*, int, int);
		virtual ~VertexBuffer();

		void SetVertexBuffer();
	private:
		void CreateDefaultHeap();
		void CreateUploadHeap();
		void CopyVertexBufferData();

		int m_size;
		int m_offset;
		ID3D12Resource* m_defaultHeap;
		ID3D12Resource* m_uploadHeap;
		void* m_bufferData;
		D3D12_VERTEX_BUFFER_VIEW m_bufferView;
	};
}
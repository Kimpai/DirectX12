#pragma once
#include <assert.h>
#include <frame.h>

#include "Direct3D12.h"

namespace GoodaCore
{
	class DescriptorHeap
	{
	public:
		DescriptorHeap(UINT numOfDescriptors, bool shaderVisible, D3D12_DESCRIPTOR_HEAP_TYPE type);
		virtual ~DescriptorHeap() = default;

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUIncrementHandle(UINT frameIndex, UINT descriptorIndex);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUIncrementHandle(UINT frameIndex, UINT descriptorIndex);
		void AppendDescriptorToHeap(D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc);
		int GetDescriptorHeapSize();
		ID3D12DescriptorHeap* GetDescriptorHeap(UINT frameIndex);

	private:
		UINT m_incrementSize;
		int m_size;
		ComPtr<ID3D12DescriptorHeap> m_descriptorHeap[frameBufferCount];
	};
}
#pragma once

#include "Light.h"

namespace GoodaCore
{
	class DirectionalLight : public Light
	{
	public:
		DirectionalLight(ID3D12Device*, ID3D12GraphicsCommandList*, XMFLOAT4, XMFLOAT4, XMFLOAT4, XMFLOAT3);
		~DirectionalLight();
		ConstantBuffer* GetConstantBuffer();
		void Render(ID3D12GraphicsCommandList*, int, int, CD3DX12_GPU_DESCRIPTOR_HANDLE);

	private:
		void InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*);
	};
}
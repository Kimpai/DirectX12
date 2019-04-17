#pragma once
#include <d3dx12.h>
#include <vector>
#include <wrl.h>
#include <assert.h>
#include "RootParameter.h"

using namespace Microsoft::WRL;

namespace GoodaCore
{
	class RootSignature
	{
	public:
		RootSignature(ID3D12Device* device, UINT size, UINT staticSamplers, RootParameter* rootParameter,
			D3D12_STATIC_SAMPLER_DESC* samplers, D3D12_ROOT_SIGNATURE_FLAGS flags);

		~RootSignature();

		void SetRootSignature();
		ID3D12RootSignature* GetRootSignature();

	private:
		ComPtr<ID3D12RootSignature> m_rootSignature;
	};
}
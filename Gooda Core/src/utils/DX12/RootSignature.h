#pragma once

#include <vector>

#include "RootParameter.h"
#include "Direct3D12.h"

namespace GoodaCore
{
	class RootSignature
	{
	public:
		RootSignature(UINT size, UINT staticSamplers, RootParameter* rootParameter,
			D3D12_STATIC_SAMPLER_DESC* samplers, D3D12_ROOT_SIGNATURE_FLAGS flags);

		virtual ~RootSignature() = default;

		void SetRootSignature();
		ID3D12RootSignature* GetRootSignature();

	private:
		ComPtr<ID3D12RootSignature> m_rootSignature;
	};
}
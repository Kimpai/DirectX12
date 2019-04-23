#include "RootSignature.h"

namespace GoodaCore
{
	RootSignature::RootSignature(UINT size, UINT staticSamplers, RootParameter* rootParameter,
		D3D12_STATIC_SAMPLER_DESC* samplers, D3D12_ROOT_SIGNATURE_FLAGS flags)
	{
		ID3DBlob* rootsignature;
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		ZeroMemory(&rootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
		rootSignatureDesc.Flags = flags;
		rootSignatureDesc.NumParameters = size;
		rootSignatureDesc.pParameters = &rootParameter[0].GetRootParameter();
		rootSignatureDesc.NumStaticSamplers = 0;
		rootSignatureDesc.pStaticSamplers = samplers;

		assert(!D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootsignature, NULL));

		assert(!Direct3D12::Instance()->GetDevice()->CreateRootSignature(0, rootsignature->GetBufferPointer(), rootsignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

		m_rootSignature->SetName(L"Root Signature");

		if (rootsignature)
			rootsignature->Release();
	}

	void RootSignature::SetRootSignature()
	{
		Direct3D12::Instance()->GetCommandList()->SetGraphicsRootSignature(m_rootSignature.Get());
	}

	ID3D12RootSignature* RootSignature::GetRootSignature()
	{
		return m_rootSignature.Get();
	}
}
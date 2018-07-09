#include "RootSignature.h"

RootSignature::RootSignature(ID3D12Device* device, UINT size, UINT staticSamplers, RootParameter* rootParameter, 
							D3D12_STATIC_SAMPLER_DESC* samplers, D3D12_ROOT_SIGNATURE_FLAGS flags)
{
	ID3DBlob* rootsignature;
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(size, &rootParameter[0].GetRootParameter(), 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	assert(!D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootsignature, NULL));

	assert(!device->CreateRootSignature(0, rootsignature->GetBufferPointer(), rootsignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

	m_rootSignature->SetName(L"Root Signature");

	if (rootsignature)
		rootsignature->Release();
}

RootSignature::~RootSignature()
{
	if (m_rootSignature)
		m_rootSignature = nullptr;
}

void RootSignature::SetRootSignature()
{
	
}

ID3D12RootSignature* RootSignature::GetRootSignature()
{
	return m_rootSignature.Get();
}

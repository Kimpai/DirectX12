#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <frame.h>
#include <vector>
#include <Gooda.h>

#include "PipelineState.h"
#include "ConstantBuffer.h"
#include "Model.h"
#include "Light.h"
#include "DescriptorHeap.h"
#include "RootSignature.h"
#include "RootParameter.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class ShaderManager : public Gooda
{
public:
	ShaderManager();
	~ShaderManager();

	void Frame(int);

	ID3D12PipelineState* GetPipelineState(ShaderPipelineType);
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilViewHandle();
	ID3D12RootSignature* GetRootSignature();
	ID3D12DescriptorHeap* GetDescriptorHeap(int);

	void CreateDescriptor(ConstantBuffer* constantBuffer);
	void CreatePipelineState(ID3D12Device*, std::vector<Shader>, int, int, ShaderPipelineType);
	void CreateRootSignature(ID3D12Device* device);
	void CreateRootDescriptorTable();
	void CreateRootDescriptorHeap(ID3D12Device* device);

private:
	void CreateDepthStencil(ID3D12Device*, int, int, D3D12_DEPTH_STENCIL_DESC&);

	DescriptorHeap* m_mainDescriptorHeap;
	RootSignature* m_rootSignature;

	std::vector<RootParameter*> m_rootParameters;
	std::vector<ConstantBuffer*> m_constantBuffers;
	
	std::vector<PipelineState*> m_pipelines;
	ComPtr<ID3D12Resource> m_depthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_depthStencilDescHeap;
};

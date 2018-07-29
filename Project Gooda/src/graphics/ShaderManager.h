#pragma once

#pragma comment(lib, "d3dcompiler.lib")

#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <frame.h>
#include <vector>
#include <Gooda.h>

#include "PipelineState.h"
#include "ConstantBuffer.h"
#include "DescriptorHeap.h"
#include "RootSignature.h"
#include "RootParameter.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class ShaderManager : public Gooda
{
public:
	ShaderManager(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	~ShaderManager();

	void Frame(int);

	ID3D12PipelineState* GetPipelineState(ShaderPipelineType);
	ID3D12RootSignature* GetRootSignature();
	ID3D12DescriptorHeap* GetDescriptorHeap(int);
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilViewHandle();

	void CreateDescriptor(ConstantBuffer* constantBuffer);
	void CreatePipelineState(std::vector<Shader>, ShaderPipelineType);
	void CreateRootSignature();
	void CreateRootDescriptorTable();
	void CreateRootDescriptorHeap();

private:
	void CreateDepthStencil(D3D12_DEPTH_STENCIL_DESC&);

	DescriptorHeap* m_mainDescriptorHeap;
	RootSignature* m_rootSignature;
	ID3D12Device* m_device;
	ID3D12GraphicsCommandList* m_commandList;

	std::vector<RootParameter*> m_rootParameters;
	std::vector<ConstantBuffer*> m_constantBuffers;
	
	std::vector<PipelineState*> m_pipelines;
	ComPtr<ID3D12Resource> m_depthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_depthStencilDescHeap;
};

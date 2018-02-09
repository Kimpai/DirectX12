#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <frame.h>

#include "Shader.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class ColorShader : public Shader
{
public:
	ColorShader();
	~ColorShader();

	void CreatPipelineState(ID3D12Device*, ID3D12RootSignature*, int, int);
	ID3D12PipelineState* GetPipelineState();
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilViewHandle();
private:
	void CompileShader(ShaderType);
	void SetRootParameters();
	void CreateDepthStencil(ID3D12Device*, int, int, D3D12_DEPTH_STENCIL_DESC&);
	void CreateInputLayout(D3D12_INPUT_LAYOUT_DESC&);

	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3DBlob> m_vertexShader;
	ComPtr<ID3DBlob> m_pixelShader;
	ComPtr<ID3D12Resource> m_depthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_depthStencilDescHeap;
};

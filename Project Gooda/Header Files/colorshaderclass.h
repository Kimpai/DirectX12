#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <d3dcompiler.h>
#include <fstream>
#include <wrl.h>

using namespace Microsoft::WRL;

class ColorShaderClass
{
public:
	ColorShaderClass();
	ColorShaderClass(const ColorShaderClass&);
	~ColorShaderClass();

	bool Initialize(ID3D12Device*, HWND, int, int);
	void Shutdown();
	ID3D12PipelineState* GetPipelineState();
	ID3D12RootSignature* GetRootSignature();
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilViewHandle();

private:
	bool InitializeShader(ID3D12Device*, HWND, WCHAR*, WCHAR*, int, int);
	void ShutdownShaders();
	void OutputShaderErrorMessage(ID3DBlob*, HWND, WCHAR*);

	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3DBlob> m_vertexShader;
	ComPtr<ID3DBlob> m_pixelShader;
	ComPtr<ID3D12Resource> m_depthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_depthStencilDescHeap;
};

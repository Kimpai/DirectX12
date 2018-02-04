#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include <iostream>
#include <wrl.h>
#include <frame.h>

using namespace DirectX;
using namespace Microsoft::WRL;

class ColorShader
{
public:
	ColorShader();
	ColorShader(const ColorShader&);
	~ColorShader();

	bool Initialize(ID3D12Device*, HWND, int, int);
	void Shutdown();
	void Frame(int);
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

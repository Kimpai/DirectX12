#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <d3dcompiler.h>
#include <fstream>

class ColorShaderClass
{
public:
	ColorShaderClass();
	ColorShaderClass(const ColorShaderClass&);
	~ColorShaderClass();

	bool Initialize(ID3D12Device*, HWND);
	void Shutdown();
	void Render(ID3D12GraphicsCommandList*);
	ID3D12PipelineState* GetPipelineState();

private:
	bool InitializeShader(ID3D12Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShaders();
	void OutputShaderErrorMessage(ID3DBlob*, HWND, WCHAR*);
	void RenderShader(ID3D12GraphicsCommandList*);

	ID3D12PipelineState* m_pipelineState;
	ID3D12RootSignature* m_rootSignature;
	ID3DBlob* m_vertexShader;
	ID3DBlob* m_pixelShader;
};

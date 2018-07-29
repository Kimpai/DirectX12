#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <d3dcompiler.h>
#include <assert.h>
#include <vector>
#include <wrl.h>

using namespace Microsoft::WRL;

enum class ShaderType { VS, PS, CS, HS, DS, GS };
enum class ShaderPipelineType { COLOR, TEXTURE };

struct Shader
{
	ShaderType m_type;
	WCHAR* m_shader;

	Shader(ShaderType shadertype, WCHAR* shader) : m_type(shadertype), m_shader(shader) {};
};

class PipelineState
{
public:
	PipelineState(ID3D12Device*, std::vector<Shader>, ShaderPipelineType, D3D12_DEPTH_STENCIL_DESC*, ID3D12RootSignature*);
	~PipelineState();

	ID3D12PipelineState* GetPipelineState();
	ShaderPipelineType GetType();

private:
	void CompileShader(Shader);

	ComPtr<ID3DBlob> m_vertexShader;
	ComPtr<ID3DBlob> m_pixelShader;
	ShaderPipelineType m_type;
	ComPtr<ID3D12PipelineState> m_pipelineState;
};
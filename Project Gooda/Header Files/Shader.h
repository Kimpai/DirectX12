#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <frame.h>
#include <vector>

using namespace DirectX;
using namespace Microsoft::WRL;

struct ShaderType
{
	enum class Type { VS, PS, CS, HS, DS, GS, INVALID };
	Type m_type;
	WCHAR* m_shader;

	ShaderType(Type type, WCHAR* shader) : m_type(type), m_shader(shader) {};
};

enum class ShaderPipelineType { COLOR, TEXTURE, INVALID };

class Shader
{
public:
	struct ShaderPipeline
	{
		ShaderPipelineType m_type;
		ComPtr<ID3D12PipelineState> m_pipelineState;

		ShaderPipeline(ShaderPipelineType type, ID3D12PipelineState* pipelinestate) : m_type(type), m_pipelineState(pipelinestate) {};
	};

	Shader();
	~Shader();

	void Frame(int);

	ID3D12PipelineState* GetPipelineState(ShaderPipelineType);
	void CreatPipelineState(std::vector<ShaderType>, ID3D12Device*, int, int, ShaderPipelineType);

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilViewHandle();
	void SetRootParameter(UINT, D3D12_ROOT_PARAMETER_TYPE, D3D12_SHADER_VISIBILITY);
	std::vector<D3D12_ROOT_PARAMETER> GetRootParameters() { return m_rootParameters; };
	ID3D12RootSignature* GetRootSignature() { return m_rootSignature.Get(); };
	void CreateRootSignature(ID3D12Device*);

private:
	void CompileShader(ShaderType);
	void CreateDepthStencil(ID3D12Device*, int, int, D3D12_DEPTH_STENCIL_DESC&);

	ComPtr<ID3DBlob> m_vertexShader;
	ComPtr<ID3DBlob> m_pixelShader;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	std::vector<D3D12_ROOT_PARAMETER> m_rootParameters;
	std::vector<ShaderPipeline> m_pipelines;
	ComPtr<ID3D12Resource> m_depthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_depthStencilDescHeap;
};

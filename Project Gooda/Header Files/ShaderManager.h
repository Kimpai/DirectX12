#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <frame.h>
#include <vector>

#include "ConstantBuffer.h"

using namespace DirectX;
using namespace Microsoft::WRL;

enum class ShaderType { VS, PS, CS, HS, DS, GS };
enum class ShaderPipelineType { COLOR, TEXTURE };

class ShaderManager
{
private:
	struct ShaderPipeline
	{
		struct Shader
		{
			ShaderType m_type;
			WCHAR* m_shader;

			Shader(ShaderType shadertype, WCHAR* shader) : m_type(shadertype), m_shader(shader) {};
		};

		ShaderPipelineType m_type;
		ComPtr<ID3D12PipelineState> m_pipelineState;
		std::vector<Shader> m_shaders;

		ShaderPipeline(ShaderPipelineType shaderPipelinetype, ID3D12PipelineState* pipelinestate, std::vector<Shader> shaders) :
			m_type(shaderPipelinetype), m_pipelineState(pipelinestate), m_shaders(shaders) {};
	};

public:
	ShaderManager();
	~ShaderManager();

	void Frame(int);

	ID3D12PipelineState* GetPipelineState(ShaderPipelineType);
	void CreatPipelineState(std::vector<ShaderPipeline::Shader>, ID3D12Device*, int, int, ShaderPipelineType);

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilViewHandle();
	void AppendRootDescriptorToHeap(ConstantBuffer*);
	void CreateRootDescriptorTableRange(UINT, D3D12_DESCRIPTOR_RANGE_TYPE, UINT);
	void CreateRootDescriptor(UINT, D3D12_ROOT_PARAMETER_TYPE, D3D12_SHADER_VISIBILITY);
	void CreateRootConstant(UINT, D3D12_ROOT_PARAMETER_TYPE, D3D12_SHADER_VISIBILITY);
	ID3D12DescriptorHeap* GetDescriptorHeap(int);

	std::vector<D3D12_ROOT_PARAMETER> GetRootParameters();
	ID3D12RootSignature* GetRootSignature();
	void CreateRootSignature(ID3D12Device*);

private:
	void CompileShader(ShaderPipeline::Shader);
	void CreateDepthStencil(ID3D12Device*, int, int, D3D12_DEPTH_STENCIL_DESC&);
	void AppendRootParameter(D3D12_ROOT_PARAMETER);
	void CreateRootDescriptorHeap(ID3D12Device* device);
	void CreateRootDescriptorTableCBV();

	ComPtr<ID3DBlob> m_vertexShader;
	ComPtr<ID3DBlob> m_pixelShader;

	ComPtr<ID3D12DescriptorHeap> m_mainDescriptorHeap[frameBufferCount];
	ComPtr<ID3D12RootSignature> m_rootSignature;
	std::vector<D3D12_ROOT_PARAMETER> m_rootParameters;
	std::vector<D3D12_DESCRIPTOR_RANGE> m_rootDescriptorTableRangesCBV;
	std::vector<ConstantBuffer*> m_constantBuffers;
	
	std::vector<ShaderPipeline> m_pipelines;
	ComPtr<ID3D12Resource> m_depthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_depthStencilDescHeap;
};

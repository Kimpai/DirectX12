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

struct ShaderType
{
	enum class Type { VS, PS, CS, HS, DS, GS, INVALID };
	Type m_type;
	WCHAR* m_shader;

	ShaderType(Type type, WCHAR* shader) : m_type(type), m_shader(shader) {};
};

enum class ShaderPipelineType { COLOR, TEXTURE, INVALID };

class ShaderManager
{
public:
	struct ShaderPipeline
	{
		ShaderPipelineType m_type;
		ComPtr<ID3D12PipelineState> m_pipelineState;

		ShaderPipeline(ShaderPipelineType type, ID3D12PipelineState* pipelinestate) : m_type(type), m_pipelineState(pipelinestate) {};
	};

	ShaderManager();
	~ShaderManager();

	void Frame(int);

	ID3D12PipelineState* GetPipelineState(ShaderPipelineType);
	void CreatPipelineState(std::vector<ShaderType>, ID3D12Device*, int, int, ShaderPipelineType);

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
	void CompileShader(ShaderType);
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

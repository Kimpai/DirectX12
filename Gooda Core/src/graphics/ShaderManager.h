#pragma once

#pragma comment(lib, "d3dcompiler.lib")

#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <frame.h>
#include <vector>
#include <Gooda.h>

#include "../utils/DX12/PipelineState.h"
#include "../utils/ConstantBuffer.h"
#include "../utils/DX12/DescriptorHeap.h"
#include "../utils/DX12/RootSignature.h"
#include "../utils/DX12/RootParameter.h"
#include "Renderer.h"

using namespace DirectX;
using namespace Microsoft::WRL;

namespace GoodaCore
{
	class ShaderManager : public Gooda
	{
	public:
		virtual ~ShaderManager();

		static ShaderManager* Instance();

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
		ShaderManager();

		DescriptorHeap* m_mainDescriptorHeap;
		ComPtr<ID3D12Resource> m_depthStencilBuffer;
		ComPtr<ID3D12DescriptorHeap> m_depthStencilDescHeap;
		RootSignature* m_rootSignature;
		std::vector<RootParameter*> m_rootParameters;
		std::vector<ConstantBuffer*> m_constantBuffers;
		std::vector<PipelineState*> m_pipelines;
	};
}
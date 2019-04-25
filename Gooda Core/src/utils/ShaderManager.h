#pragma once

#pragma comment(lib, "d3dcompiler.lib")

#include <DirectXMath.h>
#include <frame.h>
#include <vector>
#include <Gooda.h>

#include "DX12/PipelineState.h"
#include "ConstantBuffer.h"
#include "DX12/DescriptorHeap.h"
#include "DX12/RootSignature.h"
#include "DX12/RootParameter.h"

using namespace DirectX;

namespace GoodaCore
{
	class ShaderManager : public Gooda
	{
	public:
		virtual ~ShaderManager() = default;

		static ShaderManager* Instance();

		virtual bool Init();
		virtual bool Frame();
		virtual bool Destroy();

		ID3D12PipelineState* GetPipelineState(ShaderPipelineType);
		ID3D12RootSignature* GetRootSignature();
		ID3D12DescriptorHeap* GetDescriptorHeap(UINT);

		void CreateDescriptor(ConstantBuffer* constantBuffer);
		void CreatePipelineState(std::vector<Shader>, ShaderPipelineType);
		void CreateRootSignature();
		void CreateRootDescriptorTable();
		void CreateRootDescriptorHeap();

	private:
		ShaderManager();

		DescriptorHeap* m_mainDescriptorHeap;
		RootSignature* m_rootSignature;
		std::vector<RootParameter*> m_rootParameters;
		std::vector<ConstantBuffer*> m_constantBuffers;
		std::vector<PipelineState*> m_pipelines;
	};
}
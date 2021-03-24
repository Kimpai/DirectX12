#pragma once

#pragma comment(lib, "d3dcompiler.lib")

#include <DirectXMath.h>
#include <frame.h>
#include <map>

#include "DX12/PipelineState.h"
#include "ConstantBuffer.h"
#include "DX12/DescriptorHeap.h"
#include "DX12/RootSignature.h"
#include "DX12/RootParameter.h"

using namespace DirectX;

namespace GoodaCore
{
	enum class ObjectType { MODEL, LIGHT, CAMERA };

	class ShaderManager : public Gooda
	{
	public:
		virtual ~ShaderManager() = default;

		static ShaderManager* Instance();

		virtual bool Init();
		virtual bool Frame(UINT);
		virtual bool Destroy();

		ID3D12PipelineState* GetPipelineState(ShaderPipelineType);
		ID3D12RootSignature* GetRootSignature();
		ID3D12DescriptorHeap* GetDescriptorHeap(UINT);

		void CreateDescriptor(ObjectType, D3D12_CONSTANT_BUFFER_VIEW_DESC*);

	private:
		ShaderManager() = default;

		void CreatePipelineState(std::vector<Shader>, ShaderPipelineType);
		void CreateRootSignature();
		void CreateRootDescriptorTable();
		void CreateRootDescriptorHeap();

		DescriptorHeap* m_mainDescriptorHeap;
		RootSignature* m_rootSignature;
		std::vector<RootParameter*> m_rootParameters;
		std::map<ObjectType, D3D12_CONSTANT_BUFFER_VIEW_DESC*> m_descriptors;
		std::vector<PipelineState*> m_pipelines;
	};
}
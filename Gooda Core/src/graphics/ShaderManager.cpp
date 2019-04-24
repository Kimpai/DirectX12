#include "ShaderManager.h"

namespace GoodaCore
{
	ShaderManager::ShaderManager()
	{
	}

	ShaderManager* ShaderManager::Instance()
	{
		static ShaderManager s_shaderManager;
		return &s_shaderManager;
	}

	bool ShaderManager::Init()
	{
		return true;
	}

	bool ShaderManager::Frame(UINT frameIndex)
	{
		m_pipelines[0]->SetPipelineState();
		m_rootSignature->SetRootSignature();

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_mainDescriptorHeap->GetDescriptorHeap(frameIndex) };
		Direct3D12::Instance()->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		D3D12_GPU_DESCRIPTOR_HANDLE handle(ShaderManager::Instance()->GetDescriptorHeap(frameIndex)->GetGPUDescriptorHandleForHeapStart());
		Direct3D12::Instance()->GetCommandList()->SetGraphicsRootDescriptorTable(0, handle);

		return true;
	}

	bool ShaderManager::Destroy()
	{
		for (auto pipline : m_pipelines)
			if (pipline)
				delete pipline;

		if (m_mainDescriptorHeap)
			delete m_mainDescriptorHeap;

		for (int i = 0; i < m_rootParameters.size(); i++)
			if (m_rootParameters[i])
				delete m_rootParameters[i];

		if (m_rootSignature)
			delete m_rootSignature;

		return true;
	}

	ID3D12RootSignature* ShaderManager::GetRootSignature()
	{
		return m_rootSignature->GetRootSignature();
	}

	void ShaderManager::CreateDescriptor(ConstantBuffer * constantBuffer)
	{
		m_constantBuffers.push_back(constantBuffer);
	}

	void ShaderManager::CreatePipelineState(std::vector<Shader> shaders, ShaderPipelineType type)
	{
		m_pipelines.push_back(new PipelineState(shaders, type, m_rootSignature->GetRootSignature()));
	}

	void ShaderManager::CreateRootSignature()
	{
		CreateRootDescriptorTable();

		m_rootSignature = new RootSignature((UINT)m_rootParameters.size(), 0, m_rootParameters[0], NULL, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		CreateRootDescriptorHeap();
	}

	ID3D12DescriptorHeap* ShaderManager::GetDescriptorHeap(UINT frameIndex)
	{
		return m_mainDescriptorHeap->GetDescriptorHeap(frameIndex);
	}

	ID3D12PipelineState* ShaderManager::GetPipelineState(ShaderPipelineType type)
	{
		for (auto pipeline : m_pipelines)
			if (pipeline->GetType() == type)
				return pipeline->GetPipelineState();

		return nullptr;
	}

	void ShaderManager::CreateRootDescriptorTable()
	{
		D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
		m_rootParameters.push_back(new RootParameter(descriptorTable, 0, D3D12_SHADER_VISIBILITY_ALL, ParameterType::CBV, (UINT)m_constantBuffers.size(), 0));
	}

	void ShaderManager::CreateRootDescriptorHeap()
	{
		m_mainDescriptorHeap = new DescriptorHeap((UINT)m_constantBuffers.size(), true, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for (int j = 0; j < m_constantBuffers.size(); j++)
			m_mainDescriptorHeap->AppendDescriptorToHeap(m_constantBuffers[j]->GetConstantBufferViewDesc(0));
	}
}
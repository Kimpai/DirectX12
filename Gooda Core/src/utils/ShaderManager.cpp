#include "ShaderManager.h"

namespace GoodaCore
{
	ShaderManager* ShaderManager::Instance()
	{
		static ShaderManager s_shaderManager;
		return &s_shaderManager;
	}

	bool ShaderManager::Init()
	{
		CreateRootSignature();
		CreatePipelineState({ { ShaderType::VS, L"shaders/ColorVertexShader.hlsl" }, { ShaderType::PS, L"shaders/ColorPixelShader.hlsl" } },
			ShaderPipelineType::COLOR);

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

		m_descriptors.clear();

		return true;
	}

	ID3D12RootSignature* ShaderManager::GetRootSignature()
	{
		return m_rootSignature->GetRootSignature();
	}

	void ShaderManager::CreateDescriptor(ObjectType type, D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc)
	{
		if (m_descriptors.count(type) == 0)
			m_descriptors[type] = cbvDesc;
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
		m_rootParameters.push_back(new RootParameter(descriptorTable, 0, D3D12_SHADER_VISIBILITY_ALL, ParameterType::CBV, (UINT)m_descriptors.size(), 0));
	}

	void ShaderManager::CreateRootDescriptorHeap()
	{
		m_mainDescriptorHeap = new DescriptorHeap((UINT)m_descriptors.size(), true, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		std::map<ObjectType, D3D12_CONSTANT_BUFFER_VIEW_DESC*>::iterator it;

		for (it = m_descriptors.begin(); it !=  m_descriptors.end(); it++)
			m_mainDescriptorHeap->AppendDescriptorToHeap(it->second);
	}
}
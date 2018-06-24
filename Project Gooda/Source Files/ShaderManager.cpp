#include "ShaderManager.h"

ShaderManager::ShaderManager()
{
	
}

ShaderManager::~ShaderManager()
{

}

void ShaderManager::Frame(int frameIndex)
{

}

ID3D12RootSignature* ShaderManager::GetRootSignature()
{
	return m_rootSignature.Get();
}

void ShaderManager::CreateRootSignature(ID3D12Device* device)
{
	CreateRootDescriptorTable();

	ID3DBlob* rootsignature;
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init((UINT)m_rootParameters.size(), &m_rootParameters[0], 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	assert(!D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootsignature, NULL));

	assert(!device->CreateRootSignature(0, rootsignature->GetBufferPointer(), rootsignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

	m_rootSignature->SetName(L"Root Signature");

	if (rootsignature)
		rootsignature->Release();

	CreateRootDescriptorHeap(device);
}

ID3D12DescriptorHeap * ShaderManager::GetDescriptorHeap(int frameIndex)
{
	return m_mainDescriptorHeap[frameIndex].Get();
}

ID3D12PipelineState* ShaderManager::GetPipelineState(ShaderPipelineType type)
{
	for (auto pipeline : m_pipelines)
		if (pipeline.m_type == type)
			return pipeline.m_pipelineState.Get();

	return nullptr;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE ShaderManager::GetDepthStencilViewHandle()
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_depthStencilDescHeap->GetCPUDescriptorHandleForHeapStart());
}

void ShaderManager::AppendRootDescriptorToHeap(ConstantBuffer* constantBuffer)
{
	m_constantBuffers.push_back(constantBuffer);
}

void ShaderManager::CompileShader(ShaderPipeline::Shader shader)
{
	switch (shader.m_type)
	{
	case ShaderType::VS:
		assert(!D3DCompileFromFile(shader.m_shader, NULL, NULL, "main", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_vertexShader, NULL));
		break;
	case ShaderType::PS:
		assert(!D3DCompileFromFile(shader.m_shader, NULL, NULL, "main", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_pixelShader, NULL));
		break;
	default:
		assert("Invalid shader type");
		break;
	}

}

void ShaderManager::CreatPipelineState(std::vector<ShaderPipeline::Shader> shaders, ID3D12Device* device, int screenWidth, int screenHeight, ShaderPipelineType shaderPipelineType)
{
	//Compile the necessary shaders
	for (auto shader : shaders)
		CompileShader(shader);

	//Fill out a shader bytecode structure, which is basically just a pointer
	//to the shader bytecode and the size of the shader bytecode
	D3D12_SHADER_BYTECODE vertexShaderByteCode = {};
	vertexShaderByteCode.BytecodeLength = m_vertexShader->GetBufferSize();
	vertexShaderByteCode.pShaderBytecode = m_vertexShader->GetBufferPointer();

	//Fill out the shader byteccode structure for the pixel shader
	D3D12_SHADER_BYTECODE pixelShaderByteCode = {};
	pixelShaderByteCode.BytecodeLength = m_pixelShader->GetBufferSize();
	pixelShaderByteCode.pShaderBytecode = m_pixelShader->GetBufferPointer();

	//Create a depth stencil
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
	CreateDepthStencil(device, screenWidth, screenHeight, depthStencilDesc);

	//Create input layout
	//The input layout is used by the Input Assembler so that it knows
	//how to read the vertex data bound to it
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	//Fill out an input layout desc structure
	//Get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumElements = sizeof(inputElementDesc) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputElementDesc;

	//Fill in the pipeline state object desc
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};
	pipelineStateDesc.InputLayout = inputLayoutDesc;
	pipelineStateDesc.pRootSignature = GetRootSignature();
	pipelineStateDesc.VS = vertexShaderByteCode;
	pipelineStateDesc.PS = pixelShaderByteCode;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineStateDesc.DepthStencilState = depthStencilDesc;
	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.SampleDesc.Quality = 0;
	pipelineStateDesc.SampleMask = 0xffffffff;
	pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineStateDesc.NumRenderTargets = 1;

	//Create a pipeline state object
	ID3D12PipelineState* pipelineState = nullptr;
	assert(!device->CreateGraphicsPipelineState(&pipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&pipelineState));
	pipelineState->SetName(L"Color PipelineState");

	ShaderPipeline pipeline(shaderPipelineType, pipelineState, shaders);
	m_pipelines.push_back(pipeline);
}

void ShaderManager::CreateDepthStencil(ID3D12Device* device, int screenWidth, int screenHeight, D3D12_DEPTH_STENCIL_DESC& depthStencilDesc)
{
	//Fill out a depth stencil desc structure
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

	//Fill out a stencil operation structure
	D3D12_DEPTH_STENCILOP_DESC depthStencilOPDesc = {};
	depthStencilOPDesc.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilOPDesc.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilOPDesc.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilOPDesc.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	depthStencilDesc.FrontFace = depthStencilOPDesc;
	depthStencilDesc.BackFace = depthStencilOPDesc;

	//Create a depth stencil descriptor heap so we can get a pointer to the depth stencil buffer
	D3D12_DESCRIPTOR_HEAP_DESC depthStencilViewHeapDesc = {};
	depthStencilViewHeapDesc.NumDescriptors = 1;
	depthStencilViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	depthStencilViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	assert(!device->CreateDescriptorHeap(&depthStencilViewHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_depthStencilDescHeap));

	//Fill out a depth stencil desc structure
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	//Fil out a depth clear value sturcture
	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	//Create a resource and the resource heap to store the resource 
	assert(!device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, screenWidth, screenHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE,
			D3D12_TEXTURE_LAYOUT_UNKNOWN, 0),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue, __uuidof(ID3D12Resource), (void**)&m_depthStencilBuffer));

	//Give the resource a name for debugging purposes
	m_depthStencilDescHeap->SetName(L"Depth/Stencil Resource Heap");

	//Create the depth stencil view
	device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, m_depthStencilDescHeap->GetCPUDescriptorHandleForHeapStart());
}

void ShaderManager::CreateRootDescriptorTable()
{
	CreateRootDescriptorTableRange(2, 0);

	// create a descriptor table
	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = (UINT)m_descriptorRanges.size();
	descriptorTable.pDescriptorRanges = &m_descriptorRanges[0];

	D3D12_ROOT_PARAMETER rootParameter[1];
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].DescriptorTable = descriptorTable;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	AppendRootParameter(rootParameter[0]);
}

void ShaderManager::CreateRootDescriptorTableRange(UINT numOfConstantBuffers, UINT shaderRegister)
{
	D3D12_DESCRIPTOR_RANGE range[1];
	range[0].BaseShaderRegister = shaderRegister;
	range[0].NumDescriptors = numOfConstantBuffers;
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	range[0].RegisterSpace = 0;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	m_descriptorRanges.push_back(range[0]);
}

void ShaderManager::AppendRootParameter(D3D12_ROOT_PARAMETER rootParameter)
{
	m_rootParameters.push_back(rootParameter);
}

void ShaderManager::CreateRootDescriptorHeap(ID3D12Device* device)
{
	for (int i = 0; i < frameBufferCount; ++i)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = (UINT)m_constantBuffers.size();
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		assert(!device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_mainDescriptorHeap[i])));

		m_mainDescriptorHeap[i]->SetName(L"Descriptor Heap");
		for (int j = 0; j < m_constantBuffers.size(); ++j)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_mainDescriptorHeap[i]->GetCPUDescriptorHandleForHeapStart(), j, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
			device->CreateConstantBufferView(&m_constantBuffers[j]->GetConstantBufferViewDesc(i), handle);
		}
	}
}


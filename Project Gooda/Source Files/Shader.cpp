#include "Shader.h"

Shader::Shader()
{
	
}

Shader::~Shader()
{

}

void Shader::Frame(int frameIndex)
{

}

void Shader::CreateRootDescriptorTableRange(UINT numberOfDescriptors, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, UINT shaderRegister)
{
	D3D12_DESCRIPTOR_RANGE ranges[1];
	ranges[0].BaseShaderRegister = shaderRegister;
	ranges[0].NumDescriptors = numberOfDescriptors;
	ranges[0].RangeType = rangeType;
	ranges[0].RegisterSpace = 0;
	ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	m_rootDescriptorTableRangesCBV.push_back(ranges[0]);
}

void Shader::CreateRootDescriptor(UINT shaderRegister, D3D12_ROOT_PARAMETER_TYPE type, D3D12_SHADER_VISIBILITY shader)
{
	//Fill out a root desc
	D3D12_ROOT_DESCRIPTOR rootDesc;
	rootDesc.RegisterSpace = 0;
	rootDesc.ShaderRegister = shaderRegister;

	//Fill out a root parameter
	D3D12_ROOT_PARAMETER rootParameter[1];
	rootParameter[0].ParameterType = type;
	rootParameter[0].Descriptor = rootDesc;
	rootParameter[0].ShaderVisibility = shader;

	AppendRootParameter(rootParameter[0]);
}

void Shader::CreateRootConstant(UINT, D3D12_ROOT_PARAMETER_TYPE, D3D12_SHADER_VISIBILITY)
{
}

ID3D12DescriptorHeap* Shader::GetDescriptorHeap(int currentFrame)
{
	return m_mainDescriptorHeap[currentFrame].Get();
}

std::vector<D3D12_ROOT_PARAMETER> Shader::GetRootParameters()
{
	return m_rootParameters;
}

ID3D12RootSignature* Shader::GetRootSignature()
{
	return m_rootSignature.Get();
}

void Shader::CreateRootSignature(ID3D12Device* device)
{
	CreateRootDescriptorTableCBV();
	CreateRootDescriptorHeap(device);

	ComPtr<ID3DBlob> rootsignature;

	//Create root signature
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init((UINT)m_rootParameters.size(), &m_rootParameters[0], 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	assert(!D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootsignature, NULL));

	assert(!device->CreateRootSignature(0, rootsignature->GetBufferPointer(), rootsignature->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.GetAddressOf())));

	m_rootSignature->SetName(L"Root Signature");
}

ID3D12PipelineState* Shader::GetPipelineState(ShaderPipelineType type)
{
	for (auto pipeline : m_pipelines)
		if (pipeline.m_type == type)
			return pipeline.m_pipelineState.Get();

	return nullptr;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Shader::GetDepthStencilViewHandle()
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_depthStencilDescHeap->GetCPUDescriptorHandleForHeapStart());
}

void Shader::AppendRootDescriptorToHeap(ConstantBuffer* constantBuffer)
{
	m_constantBuffers.push_back(constantBuffer);
}

void Shader::CompileShader(ShaderType shader)
{
	switch (shader.m_type)
	{
	case ShaderType::Type::VS:
		assert(!D3DCompileFromFile(shader.m_shader, NULL, NULL, "main", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_vertexShader, NULL));
		break;
	case ShaderType::Type::PS:
		assert(!D3DCompileFromFile(shader.m_shader, NULL, NULL, "main", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_pixelShader, NULL));
		break;
	default:
		assert(("Invalid shader type", shader.m_type != ShaderType::Type::INVALID));
		break;
	}

}

void Shader::CreatPipelineState(std::vector<ShaderType> shaderTypes, ID3D12Device* device, int screenWidth, int screenHeight, ShaderPipelineType type)
{
	//Compile the necessary shaders
	for (auto shaderType : shaderTypes)
		CompileShader(shaderType);

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

	ShaderPipeline pipeline(type, pipelineState);
	m_pipelines.push_back(pipeline);
}

void Shader::CreateDepthStencil(ID3D12Device* device, int screenWidth, int screenHeight, D3D12_DEPTH_STENCIL_DESC& depthStencilDesc)
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

void Shader::AppendRootParameter(D3D12_ROOT_PARAMETER rootParameter)
{
	m_rootParameters.push_back(rootParameter);
}

void Shader::CreateRootDescriptorTableCBV()
{
	// create a descriptor table
	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = (UINT)m_rootDescriptorTableRangesCBV.size();
	descriptorTable.pDescriptorRanges = &m_rootDescriptorTableRangesCBV[0];

	D3D12_ROOT_PARAMETER  rootParameters[1];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].DescriptorTable = descriptorTable;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	AppendRootParameter(rootParameters[0]);
}

void Shader::CreateRootDescriptorHeap(ID3D12Device* device)
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
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			cbvDesc.BufferLocation = m_constantBuffers[j]->GetBufferLocation(i);
			cbvDesc.SizeInBytes = m_constantBuffers[j]->GetConstantBufferSize();
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_mainDescriptorHeap[i]->GetCPUDescriptorHandleForHeapStart(), j, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
			device->CreateConstantBufferView(&cbvDesc, handle);
		}
	}
}


#include "Colorshader.h"

ColorShader::ColorShader()
{
	m_pipelineState = nullptr;
	m_rootSignature = nullptr;
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilDescHeap = nullptr;
}

ColorShader::ColorShader(const ColorShader& other)
{

}

ColorShader::~ColorShader()
{

}

void ColorShader::Initialize(ID3D12Device* device, HWND hwnd, int screenHeight, int screenWidth)
{
	//Initialize the vertex and pixel shaders
	InitializeShader(device, hwnd, L"Shader Files/ColorVertexShader.hlsl", L"Shader Files/ColorPixelShader.hlsl", screenHeight, screenWidth);
}

void ColorShader::Shutdown()
{
	//Shutdown the vertex and pixel shaders
	//and all related objects
	ShutdownShaders();

	return;
}

void ColorShader::Frame(int frameIndex)
{
	return;
}

ID3D12PipelineState* ColorShader::GetPipelineState()
{
	return m_pipelineState.Get();
}

ID3D12RootSignature* ColorShader::GetRootSignature()
{
	return m_rootSignature.Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE ColorShader::GetDepthStencilViewHandle()
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_depthStencilDescHeap->GetCPUDescriptorHandleForHeapStart());
}

void ColorShader::InitializeShader(ID3D12Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, int height, int width)
{
	ComPtr<ID3DBlob> rootsignature;
	ComPtr<ID3DBlob> errorMessage;


	//Create vertex and pixel shaders
	assert(!D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_vertexShader, &errorMessage));

	//Fill out a shader bytecode structure, which is basically just a pointer
	//to the shader bytecode and the size of the shader bytecode
	D3D12_SHADER_BYTECODE vertexShaderByteCode = {};
	vertexShaderByteCode.BytecodeLength = m_vertexShader->GetBufferSize();
	vertexShaderByteCode.pShaderBytecode = m_vertexShader->GetBufferPointer();

	//Compile pixel shader
	assert(!D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_pixelShader, &errorMessage));

	//Fill out the shader byteccode structure for the pixel shader
	D3D12_SHADER_BYTECODE pixelShaderByteCode = {};
	pixelShaderByteCode.BytecodeLength = m_pixelShader->GetBufferSize();
	pixelShaderByteCode.pShaderBytecode = m_pixelShader->GetBufferPointer();

	//Fill out a descriptor range
	D3D12_ROOT_DESCRIPTOR rootCBVDesc;
	rootCBVDesc.RegisterSpace = 0;
	rootCBVDesc.ShaderRegister = 0;

	//Fill out a root parameter
	D3D12_ROOT_PARAMETER rootParameters[1];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = rootCBVDesc;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	//Create root signature
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | 
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);
	assert(!D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootsignature, &errorMessage));

	assert(!device->CreateRootSignature(0, rootsignature->GetBufferPointer(), rootsignature->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_rootSignature));

	m_rootSignature->SetName(L"Root Signature");

	//Fill out a depth stencil desc structure
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
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
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE,
			D3D12_TEXTURE_LAYOUT_UNKNOWN, 0),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue, __uuidof(ID3D12Resource), (void**)&m_depthStencilBuffer));

	//Give the resource a name for debugging purposes
	m_depthStencilDescHeap->SetName(L"Depth/Stencil Resource Heap");

	//Create the depth stencil view
	device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, m_depthStencilDescHeap->GetCPUDescriptorHandleForHeapStart());

	//Create input layout
	//The input layout is used by the Input Assembler so that it knows
	//how to read the vertex data bound to it
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	//Fill out an input layout desc structure
	//Get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumElements = sizeof(inputElementDesc) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputElementDesc;

	//Fill in the pipeline state object desc
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};
	pipelineStateDesc.InputLayout = inputLayoutDesc;
	pipelineStateDesc.pRootSignature = m_rootSignature.Get();
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
	assert(!device->CreateGraphicsPipelineState(&pipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pipelineState));
}

void ColorShader::ShutdownShaders()
{
	//Release the vertex shader
	if (m_vertexShader)
		m_vertexShader = nullptr;

	//Release the pixel shader
	if (m_pixelShader)
		m_pixelShader = nullptr;

	//Release the pipeline state
	if (m_pipelineState)
		m_pipelineState = nullptr;

	//Release the rootsignature
	if (m_rootSignature)
		m_rootSignature = nullptr;

	//Release the depth stencil buffer
	if (m_depthStencilBuffer)
		m_depthStencilBuffer = nullptr;

	//Release the depth stencil descriptor heap
	if (m_depthStencilDescHeap)
		m_depthStencilDescHeap = nullptr;
}

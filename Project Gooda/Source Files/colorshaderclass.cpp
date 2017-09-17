#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass()
{
	m_pipelineState = nullptr;
	m_rootSignature = nullptr;
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilDescHeap = nullptr;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{

}

ColorShaderClass::~ColorShaderClass()
{

}

bool ColorShaderClass::Initialize(ID3D12Device* device, HWND hwnd, int screenHeight, int screenWidth)
{
	bool result;

	//Initialize the vertex and pixel shaders
	result = InitializeShader(device, hwnd, L"Shader Files/ColorVertexShader.hlsl", L"Shader Files/ColorPixelShader.hlsl", screenHeight, screenWidth);
	if (!result)
	{
		return false;
	}

	return true;
}

void ColorShaderClass::Shutdown()
{
	//Shutdown the vertex and pixel shaders
	//and all related objects
	ShutdownShaders();

	return;
}

ID3D12PipelineState * ColorShaderClass::GetPipelineState()
{
	return m_pipelineState.Get();
}

ID3D12RootSignature* ColorShaderClass::GetRootSignature()
{
	return m_rootSignature.Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE ColorShaderClass::GetDepthStencilViewHandle()
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_depthStencilDescHeap->GetCPUDescriptorHandleForHeapStart());
}

bool ColorShaderClass::InitializeShader(ID3D12Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, int height, int width)
{
	HRESULT result;
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	D3D12_SHADER_BYTECODE vertexShaderByteCode = {};
	D3D12_SHADER_BYTECODE pixelShaderByteCode = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
	D3D12_DEPTH_STENCILOP_DESC depthStencilOPDesc = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	D3D12_DESCRIPTOR_HEAP_DESC depthStencilViewHeapDesc = {};
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	ComPtr<ID3DBlob> rootsignature;
	ComPtr<ID3DBlob> errorMessage;

	//Create root signature
	rootSignatureDesc.Init(0, NULL, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootsignature, NULL);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateRootSignature(0, rootsignature->GetBufferPointer(), rootsignature->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_rootSignature);
	if (FAILED(result))
	{
		return false;
	}

	//Create vertex and pixel shaders
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_vertexShader, &errorMessage);
	if (FAILED(result))
	{
		//If the shader failed to compile it should have written something to the error message
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage.Get(), hwnd, vsFilename);
		//If there was nothing written then it could simply not find the shader file
		else
			MessageBox(hwnd, (LPCSTR)vsFilename, (LPCSTR)L"Missing Shader File", MB_OK);
	}

	//Fill out a shader bytecode structure, which is basically just a pointer
	//to the shader bytecode and the size of the shader bytecode
	vertexShaderByteCode.BytecodeLength = m_vertexShader->GetBufferSize();
	vertexShaderByteCode.pShaderBytecode = m_vertexShader->GetBufferPointer();

	//Compile pixel shader
	result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &m_pixelShader, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage.Get(), hwnd, psFilename);
		else
			MessageBox(hwnd, (LPCSTR)psFilename, (LPCSTR)L"Missing Shader File", MB_OK);
	}

	//Fill out the shader byteccode structure for the pixel shader
	pixelShaderByteCode.BytecodeLength = m_pixelShader->GetBufferSize();
	pixelShaderByteCode.pShaderBytecode = m_pixelShader->GetBufferPointer();

	//Fill out a depth stencil desc structure
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

	//Fill out a stencil operation structure
	depthStencilOPDesc.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilOPDesc.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilOPDesc.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilOPDesc.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	depthStencilDesc.FrontFace = depthStencilOPDesc;
	depthStencilDesc.BackFace = depthStencilOPDesc;

	//Create a depth stencil descriptor heap so we can get a pointer to the depth stencil buffer
	depthStencilViewHeapDesc.NumDescriptors = 1;
	depthStencilViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	depthStencilViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	result = device->CreateDescriptorHeap(&depthStencilViewHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_depthStencilDescHeap);
	if (FAILED(result))
	{
		return false;
	}

	//Fill out a depth stencil desc structure
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	//Fil out a depth clear value sturcture
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	//Create a resource and the resource heap to store the resource 
	result = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE,
			D3D12_TEXTURE_LAYOUT_UNKNOWN, 0),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue, __uuidof(ID3D12Resource), (void**)&m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

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
	inputLayoutDesc.NumElements = sizeof(inputElementDesc) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputElementDesc;

	//Fill in the pipeline state object desc
	pipelineStateDesc.InputLayout = inputLayoutDesc;
	pipelineStateDesc.pRootSignature = m_rootSignature.Get();
	pipelineStateDesc.VS = vertexShaderByteCode;
	pipelineStateDesc.PS = pixelShaderByteCode;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineStateDesc.DepthStencilState = depthStencilDesc;
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.SampleDesc.Quality = 0;
	pipelineStateDesc.SampleMask = 0xffffffff;
	pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineStateDesc.NumRenderTargets = 1;

	//Create a pipeline state object
	result = device->CreateGraphicsPipelineState(&pipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pipelineState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShaders()
{
	//Release the vertex shader
	if (m_vertexShader)
	{
		m_vertexShader = nullptr;
	}

	//Release the pixel shader
	if (m_pixelShader)
	{
		m_pixelShader = nullptr;
	}

	//Release the pipeline state
	if (m_pipelineState)
	{
		m_pipelineState = nullptr;
	}

	//Release the rootsignature
	if (m_rootSignature)
	{
		m_rootSignature = nullptr;
	}

	return;
}

void ColorShaderClass::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize;
	std::ofstream fout;

	//Get a pointer to the error message text buffer
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//Get the length of the error message
	bufferSize = errorMessage->GetBufferSize();

	//Open a file to write the error message
	fout.open("shader-errors.txt");

	//Write out the error message
	for (unsigned long long i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	//Close the file
	fout.close();

	//Release the error message
	errorMessage->Release();
	errorMessage = nullptr;

	//Pop a message up on the screen to notify the user to check the text file for compile errors
	MessageBox(hwnd, (LPCSTR)L"Error compiling shader, check shader-errors.txt", (LPCSTR)shaderFilename, MB_OK);

	return;
}

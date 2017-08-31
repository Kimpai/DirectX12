#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass()
{
	m_pipelineState = nullptr;
	m_rootSignature = nullptr;
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{

}

ColorShaderClass::~ColorShaderClass()
{

}

bool ColorShaderClass::Initialize(ID3D12Device* device, HWND hwnd)
{
	bool result;

	//Initialize the vertex and pixel shaders
	result = InitializeShader(device, hwnd, L"Shader Files/ColorVertexShader.hlsl", L"Shader Files/ColorPixelShader.hlsl");
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

void ColorShaderClass::Render(ID3D12GraphicsCommandList* commandList)
{
	RenderShader(commandList);
}

ID3D12PipelineState* ColorShaderClass::GetPipelineState()
{
	return m_pipelineState;
}

bool ColorShaderClass::InitializeShader(ID3D12Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	D3D12_SHADER_BYTECODE vertexShaderByteCode = {};
	D3D12_SHADER_BYTECODE pixelShaderByteCode = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	ID3DBlob* rootsignature;
	ID3DBlob* errorMessage;

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
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
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
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		else
			MessageBox(hwnd, (LPCSTR)psFilename, (LPCSTR)L"Missing Shader File", MB_OK);
	}

	//Fill out the shader byteccode structure for the pixel shader
	pixelShaderByteCode.BytecodeLength = m_pixelShader->GetBufferSize();
	pixelShaderByteCode.pShaderBytecode = m_pixelShader->GetBufferPointer();

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
	pipelineStateDesc.pRootSignature = m_rootSignature;
	pipelineStateDesc.VS = vertexShaderByteCode;
	pipelineStateDesc.PS = pixelShaderByteCode;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
	pipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
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
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}

	//Release the pixel shader
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	//Release the pipeline state
	if (m_pipelineState)
	{
		m_pipelineState->Release();
		m_pipelineState = nullptr;
	}

	//Release the rootsignature
	if (m_rootSignature)
	{
		m_rootSignature->Release();
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

void ColorShaderClass::RenderShader(ID3D12GraphicsCommandList* commandList)
{
	//Set the root signature
	commandList->SetGraphicsRootSignature(m_rootSignature);

	//Draw
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

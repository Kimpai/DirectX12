#include "PipelineState.h"

namespace GoodaCore
{
	PipelineState::PipelineState(std::vector<Shader> shaders,
		ShaderPipelineType shaderPipelineType, ID3D12RootSignature* rootSignature)
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
		ZeroMemory(&pipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

		pipelineStateDesc.InputLayout = inputLayoutDesc;
		pipelineStateDesc.pRootSignature = rootSignature;
		pipelineStateDesc.VS = vertexShaderByteCode;
		pipelineStateDesc.PS = pixelShaderByteCode;
		pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateDesc.SampleDesc.Count = 1;
		pipelineStateDesc.SampleDesc.Quality = 0;
		pipelineStateDesc.SampleMask = 0xffffffff;
		pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pipelineStateDesc.NumRenderTargets = 1;

		//Create a pipeline state object
		Direct3D12::Instance()->GetDevice()->CreateGraphicsPipelineState(&pipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)& m_pipelineState);
		m_pipelineState->SetName(L"Color PipelineState");
	}

	void PipelineState::SetPipelineState()
	{
		Direct3D12::Instance()->GetCommandList()->SetPipelineState(m_pipelineState.Get());
	}

	ID3D12PipelineState* PipelineState::GetPipelineState()
	{
		return m_pipelineState.Get();
	}

	ShaderPipelineType PipelineState::GetType()
	{
		return m_type;
	}

	bool PipelineState::CompileShader(Shader shader)
	{
		UINT flags = D3DCOMPILE_OPTIMIZATION_LEVEL0;

#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
#endif // DEBUG

		switch (shader.m_type)
		{
		case ShaderType::VS:
			if (FAILED(D3DCompileFromFile(shader.m_shader, NULL, NULL, "main", "vs_5_0", flags, 0, &m_vertexShader, NULL)))
				return false;

			break;
		case ShaderType::PS:
			if (FAILED(D3DCompileFromFile(shader.m_shader, NULL, NULL, "main", "ps_5_0", flags, 0, &m_pixelShader, NULL)))
				return false;

			break;
		default:
			//"Invalid shader type"
			break;
		}

		return true;
	}
}
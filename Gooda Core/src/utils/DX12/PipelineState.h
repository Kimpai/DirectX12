#pragma once

#include <d3dcompiler.h>
#include <vector>
#include <wrl.h>

#include "../DX12/Direct3D12.h"

namespace GoodaCore
{
	enum class ShaderType { VS, PS, CS, HS, DS, GS };
	enum class ShaderPipelineType { COLOR, TEXTURE };

	struct Shader
	{
		ShaderType m_type;
		WCHAR* m_shader;

		Shader(ShaderType shadertype, WCHAR* shader) : m_type(shadertype), m_shader(shader) {};
	};

	class PipelineState
	{
	public:
		PipelineState(std::vector<Shader>, ShaderPipelineType, ID3D12RootSignature*);
		virtual ~PipelineState();

		void SetPipelineState();
		ID3D12PipelineState* GetPipelineState();
		ShaderPipelineType GetType();

	private:
		bool CompileShader(Shader);

		ID3DBlob* m_vertexShader;
		ID3DBlob* m_pixelShader;
		ShaderPipelineType m_type;
		ID3D12PipelineState* m_pipelineState;
	};
}
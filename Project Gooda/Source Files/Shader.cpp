#include "Shader.h"

Shader::Shader()
{

}

Shader::~Shader()
{

}

void Shader::Initialize(std::vector<ShaderType> shaderTypes)
{
	for (auto shaderType : shaderTypes)
		CompileShader(shaderType);

	SetRootParameters();
}

void Shader::Frame(int frameIndex)
{

}


#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <frame.h>
#include <vector>

using namespace DirectX;
using namespace Microsoft::WRL;

struct ShaderType
{
	enum class Type { VS, PS, CS, HS, DS, GS, INVALID };
	Type m_type;
	WCHAR* m_shader;

	ShaderType(Type type, WCHAR* shader) : m_type(type), m_shader(shader) {};
};

class Shader
{
public:
	Shader();
	~Shader();

	void Initialize(std::vector<ShaderType>);
	void Frame(int);

	virtual ID3D12PipelineState* GetPipelineState() { return nullptr; };
	virtual CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilViewHandle() { return CD3DX12_CPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT); };
	std::vector<D3D12_ROOT_PARAMETER> GetRootParameters() { return m_rootParameters; };

	std::vector<D3D12_ROOT_PARAMETER> m_rootParameters;
private:
	virtual void CompileShader(ShaderType) = 0;
	virtual void SetRootParameters() { return; };
};

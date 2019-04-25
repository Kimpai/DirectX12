#pragma once
#include <d3d12.h>
#include <vector>

namespace GoodaCore
{
	enum class ParameterType { CBV = 2, UAV = 3, SRV = 4 };

	class RootParameter
	{
	public:
		RootParameter(ParameterType type, UINT baseShaderRegister, D3D12_SHADER_VISIBILITY visibility, int rootIndex);
		RootParameter(D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable, UINT baseShaderRegister, D3D12_SHADER_VISIBILITY visibility,
			ParameterType type, UINT numOfDescriptors, int rootIndex);
		virtual ~RootParameter() = default;

		void AppendDescriptorToTable(UINT baseShaderRegister, ParameterType type, UINT numOfDescriptors);
		D3D12_ROOT_PARAMETER GetRootParameter();

	private:
		D3D12_DESCRIPTOR_RANGE GetDescriptorTableRange(UINT numOfConstantBuffers, UINT shaderRegister, ParameterType type);
		int m_rootIndex;
		D3D12_ROOT_PARAMETER m_rootParameter;
		std::vector<D3D12_DESCRIPTOR_RANGE> m_descriptorRanges;
	};
}
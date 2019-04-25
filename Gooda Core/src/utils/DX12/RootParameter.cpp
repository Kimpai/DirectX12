#include "RootParameter.h"

namespace GoodaCore
{
	RootParameter::RootParameter(ParameterType type, UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility, int rootIndex) : m_rootIndex(rootIndex)
	{
		D3D12_ROOT_PARAMETER rootParameter = {};
		rootParameter.Descriptor = { 0, shaderRegister };
		rootParameter.ShaderVisibility = visibility;
		rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE(type);

		m_rootParameter = rootParameter;
	}

	RootParameter::RootParameter(D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable, UINT baseShaderRegister,
		D3D12_SHADER_VISIBILITY visibility, ParameterType type, UINT numOfDescriptors, int rootIndex) : m_rootIndex(rootIndex)
	{

		m_descriptorRanges.push_back(GetDescriptorTableRange(numOfDescriptors, baseShaderRegister, type));

		//Create a descriptor table
		descriptorTable.NumDescriptorRanges = (UINT)m_descriptorRanges.size();
		descriptorTable.pDescriptorRanges = &m_descriptorRanges[0];

		D3D12_ROOT_PARAMETER rootParameter = {};
		rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter.DescriptorTable = descriptorTable;
		rootParameter.ShaderVisibility = visibility;

		m_rootParameter = rootParameter;
	}

	void RootParameter::AppendDescriptorToTable(UINT baseShaderRegister, ParameterType type, UINT numOfDescriptors)
	{
		m_descriptorRanges.push_back(GetDescriptorTableRange(numOfDescriptors, baseShaderRegister, type));

		//Create a descriptor table
		D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
		descriptorTable.NumDescriptorRanges = (UINT)m_descriptorRanges.size();
		descriptorTable.pDescriptorRanges = &m_descriptorRanges[0];

		D3D12_ROOT_PARAMETER rootParameter = {};
		rootParameter.ParameterType = m_rootParameter.ParameterType;
		rootParameter.DescriptorTable = descriptorTable;
		rootParameter.ShaderVisibility = m_rootParameter.ShaderVisibility;

		m_rootParameter = rootParameter;
	}

	D3D12_ROOT_PARAMETER RootParameter::GetRootParameter()
	{
		return m_rootParameter;
	}

	D3D12_DESCRIPTOR_RANGE RootParameter::GetDescriptorTableRange(UINT numOfConstantBuffers, UINT baseShaderRegister, ParameterType type)
	{
		D3D12_DESCRIPTOR_RANGE range = {};

		switch (type)
		{
		case ParameterType::CBV:
			range.BaseShaderRegister = baseShaderRegister;
			range.NumDescriptors = numOfConstantBuffers;
			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			range.RegisterSpace = 0;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			break;
		case ParameterType::UAV:
			range.BaseShaderRegister = baseShaderRegister;
			range.NumDescriptors = numOfConstantBuffers;
			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			range.RegisterSpace = 0;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			break;
		case ParameterType::SRV:
			range.BaseShaderRegister = baseShaderRegister;
			range.NumDescriptors = numOfConstantBuffers;
			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			range.RegisterSpace = 0;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			break;
		default:
			//"Invalid root parameter type"
			break;
		}

		return range;
	}
}
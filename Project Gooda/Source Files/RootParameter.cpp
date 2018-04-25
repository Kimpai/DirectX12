#include "RootParameter.h"

RootParameter::RootParameter(const UINT & shaderRegister, D3D12_SHADER_VISIBILITY visibility)
{
	D3D12_ROOT_PARAMETER1 param = {};
	param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	param.Descriptor = { 0, shaderRegister };
	param.ShaderVisibility = visibility;
}

RootParameter::RootParameter(D3D12_ROOT_DESCRIPTOR_TABLE1 table, D3D12_SHADER_VISIBILITY visibility)
{
	D3D12_ROOT_PARAMETER1 param = {};
	param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	param.DescriptorTable = table;
	param.ShaderVisibility = visibility;
}

RootParameter::RootParameter(UINT numDesc, D3D12_DESCRIPTOR_RANGE1 * pDescRanges, D3D12_SHADER_VISIBILITY visibility)
{
	D3D12_ROOT_PARAMETER1 param = {};
	param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	param.DescriptorTable.NumDescriptorRanges = numDesc;
	param.DescriptorTable.pDescriptorRanges = pDescRanges;
	param.ShaderVisibility = visibility;
}

RootParameter::~RootParameter()
{
}

#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <vector>


class RootParameter
{
public:
	RootParameter(const UINT & shaderRegister, D3D12_SHADER_VISIBILITY visibility);
	RootParameter(D3D12_ROOT_DESCRIPTOR_TABLE1 table, D3D12_SHADER_VISIBILITY visibility);
	RootParameter(UINT numDesc, D3D12_DESCRIPTOR_RANGE1* pDescRanges, D3D12_SHADER_VISIBILITY visibility);
	~RootParameter();
};
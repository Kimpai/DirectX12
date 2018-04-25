#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <vector>

class RootDescriptor
{
public:
	//Call this after you have appended all the parameters to the descriptor range
	RootDescriptor();
	~RootDescriptor();

	void AppendDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, const UINT & numDesc, const UINT & baseShaderRegister, D3D12_DESCRIPTOR_RANGE_FLAGS flags);
	std::vector<CD3DX12_DESCRIPTOR_RANGE1> GetDescRange();
	D3D12_ROOT_DESCRIPTOR_TABLE1 GetRootDescTable();

private:
	std::vector<CD3DX12_DESCRIPTOR_RANGE1> m_descRange;
	CD3DX12_ROOT_DESCRIPTOR_TABLE1 m_rootDescTable;
};

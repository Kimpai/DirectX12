#include "RootDescriptor.h"

RootDescriptor::RootDescriptor()
{
	m_rootDescTable = { (UINT)m_descRange.size(), &m_descRange[0] };
}

RootDescriptor::~RootDescriptor()
{
}

void RootDescriptor::AppendDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, const UINT & numDesc, const UINT & baseShaderRegister, D3D12_DESCRIPTOR_RANGE_FLAGS flags)
{
	m_descRange.push_back({ type, numDesc, baseShaderRegister, 0, flags });
}

std::vector<CD3DX12_DESCRIPTOR_RANGE1> RootDescriptor::GetDescRange()
{
	return m_descRange;
}

D3D12_ROOT_DESCRIPTOR_TABLE1 RootDescriptor::GetRootDescTable()
{
	return m_rootDescTable;
}

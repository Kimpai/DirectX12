#include "Light.h"

namespace GoodaCore
{
	Light::Light()
	{
	}

	bool Light::Frame(UINT frameIndex, D3D12_GPU_DESCRIPTOR_HANDLE handle)
	{
		//Update the constant buffer data
		m_constantBuffer->UpdateConstantBufferData(frameIndex);

		return true;
	}
}
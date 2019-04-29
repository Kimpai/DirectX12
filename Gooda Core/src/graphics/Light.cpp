#include "Light.h"

namespace GoodaCore
{
	Light::Light()
	{

	}

	bool Light::Frame()
	{
		UINT frameIndex = Direct3D12::Instance()->GetCurrentFrame();

		//Update the constant buffer data
		m_constantBuffer->UpdateConstantBufferData(frameIndex);

		return true;
	}
}
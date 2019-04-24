#include "Light.h"

namespace GoodaCore
{
	Light::Light()
	{
	}

	bool Light::Frame(UINT frameIndex)
	{
		//Update the constant buffer data
		m_constantBuffer->UpdateConstantBufferData(frameIndex);

		return true;
	}
}
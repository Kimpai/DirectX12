#pragma once

#include "Light.h"

namespace GoodaCore
{
	class DirectionalLight : public Light
	{
	public:
		DirectionalLight(XMFLOAT4, XMFLOAT4, XMFLOAT4, XMFLOAT3);
		virtual ~DirectionalLight();

		ConstantBuffer* GetConstantBuffer();

	private:
		void InitializeBuffers();
	};
}
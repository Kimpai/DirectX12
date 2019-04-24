#pragma once

#include "Light.h"

namespace GoodaCore
{
	class DirectionalLight : public Light
	{
	public:
		DirectionalLight(XMFLOAT4, XMFLOAT4, XMFLOAT4, XMFLOAT3);
		virtual ~DirectionalLight() = default;

		virtual bool Init();
		virtual bool Destroy();

		ConstantBuffer* GetConstantBuffer();

	private:
		void InitializeBuffers();
	};
}
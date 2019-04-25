#pragma once

#include <DirectXMath.h>
#include <Gooda.h>

#include "../utils/ConstantBuffer.h"

using namespace DirectX;

namespace GoodaCore
{
	class Light : public Gooda
	{
	public:
		ConstantBuffer* m_constantBuffer;

		struct ConstantBufferData
		{
			XMFLOAT4 ambientColor;
			XMFLOAT4 diffuseColor;
			XMFLOAT4 specularColor;
			XMFLOAT3 lightDirection;
			float specFactor = 32.0f;

		} m_constantBufferData;

		Light();
		virtual ~Light() = default;

		virtual bool Frame();
		virtual ConstantBuffer* GetConstantBuffer() = 0;

	private:
		virtual void InitializeBuffers() = 0;
	};
}
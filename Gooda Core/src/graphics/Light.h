#pragma once

#include <DirectXMath.h>

#include "../utils/ShaderManager.h"
#include "../utils/ConstantBuffer.h"

using namespace DirectX;

namespace GoodaCore
{
	class Light : public Gooda
	{
	public:
		ConstantBuffer* m_constantBuffer = nullptr;

		struct ConstantBufferData
		{
			XMFLOAT4 ambientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			XMFLOAT4 diffuseColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			XMFLOAT4 specularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			XMFLOAT3 lightDirection = XMFLOAT3(0.0f, 0.0f, 0.0f);
			float specFactor = 32.0f;

		} m_constantBufferData;

		virtual ~Light() = default;

		virtual bool Frame(UINT);
		virtual ConstantBuffer* GetConstantBuffer() = 0;

	protected:
		Light() = default;

	private:
		virtual void InitializeBuffers() = 0;
	};
}
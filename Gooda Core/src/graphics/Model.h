#pragma once

#include <DirectXMath.h>
#include <frame.h>
#include <Gooda.h>

#include "../utils/VertexBuffer.h"
#include "../utils/IndexBuffer.h"
#include "../utils/ConstantBuffer.h"

using namespace DirectX;

namespace GoodaCore
{
	class Model : public Gooda
	{
	public:
		ConstantBuffer* m_constantBuffer;
		VertexBuffer* m_vertexBuffer;
		IndexBuffer* m_indexBuffer;

		struct ConstantBufferData
		{
			XMFLOAT4X4 worldMatrix;
			XMFLOAT4X4 projectionMatrix;

		} m_constantBufferData;

		Model(XMFLOAT3);
		virtual ~Model() = default;

		virtual ConstantBuffer* GetConstantBuffer() = 0;

		virtual bool Init();
		virtual bool Frame(UINT);
		virtual bool Destroy();
		virtual void Draw() = 0;

	private:
		void UpdateMatrices();
		virtual void BuildWorlViewProjectionMatrix(XMFLOAT3);
		virtual void InitializeBuffers() = 0;

		XMFLOAT3 m_origin;
		XMFLOAT4X4 m_worldMatrix;
		XMFLOAT4X4 m_projectionMatrix;
	};
}
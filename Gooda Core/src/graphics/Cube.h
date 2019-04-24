#pragma once

#include <DirectXMath.h>
#include <VertexTypes.h>
#include <frame.h>

#include "Model.h"
#include "../utils/DX12/Direct3D12.h"

using namespace DirectX;

namespace GoodaCore
{
	class Cube : public Model
	{
	public:
		Cube(XMFLOAT3, XMFLOAT4);
		virtual ~Cube() = default;

		virtual bool Init();
		virtual bool Destroy();

		ConstantBuffer* GetConstantBuffer();
		XMFLOAT3 GetPosition();
		virtual void Draw();

	private:
		void InitializeBuffers();

		int m_indices;
		XMFLOAT3 m_origin;
		XMFLOAT4 m_color;
	};
}
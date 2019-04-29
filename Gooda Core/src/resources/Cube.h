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
		virtual ~Cube();

		virtual void Draw();

	private:
		void InitializeBuffers();

		UINT m_indices;
		XMFLOAT3 m_origin;
		XMFLOAT4 m_color;
	};
}
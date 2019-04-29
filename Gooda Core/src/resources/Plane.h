#pragma once

#include <DirectXMath.h>
#include <VertexTypes.h>
#include <fstream>

#include "Model.h"
#include "../utils/DX12/Direct3D12.h"

using namespace DirectX;

namespace GoodaCore
{
	class Plane : public Model
	{
	public:
		Plane(UINT, UINT, XMFLOAT3, XMFLOAT4);
		virtual ~Plane();

		virtual void Draw();

	private:
		void InitializeBuffers();

		UINT m_width;
		UINT m_height;
		UINT m_indices;
		XMFLOAT3 m_origin;
		XMFLOAT4 m_color;
	};
}
#pragma once

#include <DirectXMath.h>
#include <VertexTypes.h>
#include <fstream>

#include "Model.h"
#include "../utils/DX12/Direct3D12.h"

using namespace DirectX;

namespace GoodaCore
{
	class Terrain : public Model
	{
	public:
		Terrain(char*, int, int, float, XMFLOAT3);
		virtual ~Terrain();
		virtual void Draw();

	private:
		void InitializeBuffers();

		void LoadBitmapHeightMap();
		void SetCoordinates();
		void Build(VertexPositionNormalColor*);

		int m_height, m_width;
		float m_scale;
		char* m_file;
		int m_indices;

		XMFLOAT3* m_heightMap;
	};
}
#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <VertexTypes.h>
#include <wrl.h>
#include <fstream>

#include "Model.h"
#include "../utils/DX12/Direct3D12.h"

using namespace DirectX;
using namespace Microsoft::WRL;

namespace GoodaCore
{
	class Terrain : public Model
	{
	public:
		Terrain(char*, int, int, float, XMFLOAT3);
		virtual ~Terrain() = default;

		virtual bool Init();
		virtual bool Frame(UINT, XMMATRIX, D3D12_GPU_DESCRIPTOR_HANDLE);
		virtual bool Destroy();

		ConstantBuffer* GetConstantBuffer();
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
#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <VertexTypes.h>
#include <wrl.h>

#include "GoodaModel.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class Terrain : public Model
{
public:
	Terrain(int, int);
	~Terrain();

private:
	void InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*);
	void Render(ID3D12GraphicsCommandList*, int);

	int m_height, m_width;
	int m_Indices;
};
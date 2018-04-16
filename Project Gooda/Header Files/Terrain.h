#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <VertexTypes.h>
#include <wrl.h>
#include <fstream>

#include "GoodaModel.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class Terrain : public Model
{
public:
	Terrain(char*, int, int, float);
	~Terrain();

	ConstantBuffer* GetConstantBuffer();

private:
	void InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*);
	void Render(ID3D12GraphicsCommandList*, int);

	void LoadBitmapHeightMap();
	void SetCoordinates();
	void Build(VertexPositionNormalColor*);

	int m_height, m_width;
	float m_scale;
	char* m_file;
	int m_indices;

	XMFLOAT3* m_heightMap;

};
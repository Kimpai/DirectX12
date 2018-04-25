#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <VertexTypes.h>
#include <frame.h>

#include "Model.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class Cube : public Model
{
public:
	Cube(XMFLOAT3 origin, XMFLOAT4 color);
	~Cube();

	ConstantBuffer* GetConstantBuffer();
	XMFLOAT3 GetPosition();

private:
	void InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*);
	void Render(ID3D12GraphicsCommandList*, int, CD3DX12_GPU_DESCRIPTOR_HANDLE);
	
	int m_indices;
	XMFLOAT3 m_origin;
	XMFLOAT4 m_color;
};

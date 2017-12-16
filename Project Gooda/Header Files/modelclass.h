#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "cameraclass.h"
#include "frame.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class Model
{
public:
	struct ConstantBuffer
	{
		XMFLOAT4X4 worldMatrix;
		XMFLOAT4X4 viewMatrix;
		XMFLOAT4X4 projectionMatrix;
		XMFLOAT4X4 rotationMatrix;
	}m_constantBuffer;

	Model();
	Model(const Model&);
	virtual ~Model();

	bool Initialize(ID3D12Device*, ID3D12GraphicsCommandList*, Camera*, int, int, float, float, XMFLOAT4);
	void Render(ID3D12GraphicsCommandList*, int);
	void Shutdown();
	virtual void BuildWorlViewProjectionMatrix(Camera*, int , int, float, float, XMFLOAT4);
	virtual void Frame(int, Camera*);

	ComPtr<ID3D12DescriptorHeap> m_constantBufferDescHeap[frameBufferCount];
	ComPtr<ID3D12Resource> m_constantBufferUploadHeap[frameBufferCount];
	UINT8* m_constantBufferGPUAddress[frameBufferCount];
	const int ConstantBufferPerObjectAlignedSize = (sizeof(ConstantBuffer) + 255) & ~255;

private:
	virtual void ShutdownBuffers() = 0;
	virtual bool InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*, XMFLOAT4) = 0;
	virtual void RenderBuffers(ID3D12GraphicsCommandList*, int) = 0;
};
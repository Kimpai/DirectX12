#pragma once

#include <d3d12.h>
#include <d3dx12.h>

class ModelClass
{
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D12Device*, ID3D12GraphicsCommandList*);
	void Render(ID3D12GraphicsCommandList*);
	void Shutdown();

private:
	virtual void ShutdownBuffers() = 0;
	virtual bool InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*) = 0;
	virtual void RenderBuffers(ID3D12GraphicsCommandList*) = 0;
};
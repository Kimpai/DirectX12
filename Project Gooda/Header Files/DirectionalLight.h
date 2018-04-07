#pragma once

#include "Light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight(XMFLOAT4, XMFLOAT3);
	~DirectionalLight();
	void Render(ID3D12GraphicsCommandList*, int);

private:
	void InitializeBuffers(ID3D12Device*, ID3D12GraphicsCommandList*);
};
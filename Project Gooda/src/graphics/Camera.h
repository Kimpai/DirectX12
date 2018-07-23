#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <iostream>
#include <Gooda.h>

#include "Input.h"
#include "ConstantBuffer.h"

using namespace DirectX;

class Camera : public Gooda
{
public:
	Camera(Input*, ID3D12Device*, ID3D12GraphicsCommandList*);
	~Camera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	ConstantBuffer* GetConstanBuffer();
	
	void Frame(int);
	void Render(int, CD3DX12_GPU_DESCRIPTOR_HANDLE);
	XMMATRIX GetViewMatrix();
	XMMATRIX GetBaseViewMatrix();

private:
	struct ConstantBufferData
	{
		XMFLOAT3 CameraPosition;
		float padding = 0.0f;
	} m_constantBufferData;

	void BuildBaseViewMatrix();
	void BuildViewMatrix();

	void MoveForward();
	void MoveBackward();
	void MoveRight();
	void MoveLeft();
	void Turn();

	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_baseViewMatrix;

	float m_forwardSpeed, m_backwardSpeed;
	float m_rightSpeed, m_leftSpeed;
	float m_turnSpeed;
	float m_frameTime;
	XMFLOAT2 m_mouse;

	Input* m_inputHandler;
	ConstantBuffer* m_constantBuffer;
};
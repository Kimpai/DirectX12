#pragma once

#include <DirectXMath.h>
#include <iostream>
#include <Gooda.h>

#include "../Gooda/Input.h"
#include "../utils/ConstantBuffer.h"

using namespace DirectX;

namespace GoodaCore
{
	class Camera : public Gooda
	{
	public:
		Camera();
		virtual ~Camera() = default;

		void SetPosition(float, float, float);
		void SetRotation(float, float, float);

		XMFLOAT3 GetPosition();
		XMFLOAT3 GetRotation();
		ConstantBuffer* GetConstanBuffer();

		virtual bool Init();
		virtual bool Frame(UINT, D3D12_GPU_DESCRIPTOR_HANDLE);
		virtual bool Destroy();

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

		ConstantBuffer* m_constantBuffer;
	};
}
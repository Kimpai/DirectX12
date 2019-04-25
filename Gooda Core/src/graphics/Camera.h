#pragma once

#include <DirectXMath.h>
#include <iostream>
#include <Gooda.h>

#include "../Gooda/Input.h"
#include "../utils/ShaderManager.h"

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
		virtual bool Frame();
		virtual bool Destroy();

		XMFLOAT4X4 GetViewMatrix();
		XMFLOAT4X4 GetBaseViewMatrix();

	private:
		struct ConstantBufferData
		{
			XMFLOAT4X4 viewMatrix;
			XMFLOAT3 cameraPosition;
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
		XMFLOAT4X4 m_viewMatrix;
		XMFLOAT4X4 m_baseViewMatrix;

		float m_forwardSpeed, m_backwardSpeed;
		float m_rightSpeed, m_leftSpeed;
		float m_turnSpeed;
		float m_frameTime;
		XMFLOAT2 m_mouse;

		ConstantBuffer* m_constantBuffer;
	};
}
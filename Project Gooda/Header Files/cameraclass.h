#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	
	void Frame();
	void GetViewMatrix(XMMATRIX&);
	void GetBaseViewMatrix(XMMATRIX&);

	void MoveForward(bool);
	void MoveBackward(bool);
	void MoveRight(bool);
	void MoveLeft(bool);
	void Turn();

private:
	void BuildBaseViewMatrix();

	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_baseViewMatrix;

	float m_forwardSpeed, m_backwardSpeed;
	float m_rightSpeed, m_leftSpeed;
	float m_turnSpeed;
	float m_frameTime;
};
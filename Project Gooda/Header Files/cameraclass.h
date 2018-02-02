#pragma once

#include <DirectXMath.h>
#include "inputclass.h"

using namespace DirectX;

class Camera
{
public:
	Camera(Input*);
	Camera(const Camera&);
	~Camera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	
	void Frame();
	void GetViewMatrix(XMMATRIX&);
	void GetBaseViewMatrix(XMMATRIX&);

private:
	void BuildBaseViewMatrix();

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

	Input* m_inputHandler;
};
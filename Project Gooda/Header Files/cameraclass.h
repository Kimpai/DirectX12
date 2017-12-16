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
	void Frame(float, float);

	void MoveForward();
	void MoveBackward();
	void MoverLeft();
	void MoverRight();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetRight();
	XMFLOAT3 GetLookAt();
	XMFLOAT3 GetUp();
private:
	XMFLOAT4 m_position;
	XMFLOAT4 m_rotation;
	XMFLOAT4 m_right;
	XMFLOAT4 m_lookAt;
	XMFLOAT4 m_up;

	float m_pitch, m_yaw;
	float m_frameTime;
	float m_moveSpeed, m_strafeSpeed;
	float m_turnLeftRightSpeed;
	float m_lookUpDownSpeed;

	bool m_moveForward, m_moveRight;
	bool m_moveBackward, m_moveLeft;
};
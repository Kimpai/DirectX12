#include "cameraclass.h"

Camera::Camera()
{
	m_position = XMFLOAT4(0.0f, 0.0f, -2.0f, 1.0f);
	m_rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_lookAt = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	m_right = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	m_up = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	m_pitch = 0.0f;
	m_yaw = 0.0f;
	m_frameTime = 0.0f;
	m_moveSpeed = 0.0f;
	m_strafeSpeed = 0.0f;
	m_turnLeftRightSpeed = 0.05f;
	m_lookUpDownSpeed = 0.05f;

	m_moveBackward = false;
	m_moveForward = false;
	m_moveLeft = false;
	m_moveRight = false;
}

Camera::Camera(const Camera& other)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT4(x, y, z, 1.0f);
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rotation = XMFLOAT4(x, y, z, 1.0f);
}

void Camera::Frame(float mouseX, float mouseY)
{
	m_pitch += mouseY * m_lookUpDownSpeed;
	m_yaw += mouseX * m_turnLeftRightSpeed;

	XMFLOAT4 rotation(m_pitch, m_yaw, 0.0f, 1.0f);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4(&rotation));

	XMMATRIX rotationMatrixY(XMMatrixRotationY(m_rotation.y));
	XMVECTOR right = XMVector3TransformCoord(XMLoadFloat4(&m_right), rotationMatrixY);
	XMVECTOR up = XMVector3TransformCoord(XMLoadFloat4(&m_up), rotationMatrixY);
	XMVECTOR lookAt = XMVector3TransformCoord(XMLoadFloat4(&m_lookAt), rotationMatrixY);

	if (m_moveForward)
	{
		XMFLOAT4 dir(0.0f, 0.0f, 1.0f, 1.0f);
		XMVECTOR position = XMVectorMultiplyAdd(XMLoadFloat4(&dir), lookAt, XMLoadFloat4(&m_position));
		position *= m_moveSpeed;
		XMStoreFloat4(&m_position, position);
	}

	if (m_moveBackward)
	{
		XMFLOAT4 dir(0.0f, 0.0f, -1.0f, 1.0f);
		XMVECTOR position = XMVectorMultiplyAdd(XMLoadFloat4(&dir), lookAt, XMLoadFloat4(&m_position));
		position *= m_moveSpeed;
		XMStoreFloat4(&m_position, position);
	}

	if (m_moveLeft)
	{
		XMFLOAT4 dir(-1.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR position = XMVectorMultiplyAdd(XMLoadFloat4(&dir), right, XMLoadFloat4(&m_position));
		position *= m_strafeSpeed;
		XMStoreFloat4(&m_position, position);
	}

	if (m_moveRight)
	{
		XMFLOAT4 dir(1.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR position = XMVectorMultiplyAdd(XMLoadFloat4(&dir), right, XMLoadFloat4(&m_position));
		position *= m_strafeSpeed;
		XMStoreFloat4(&m_position, position);
	}

	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	lookAt = XMVector3Normalize(lookAt);
	lookAt = XMVectorAdd(XMLoadFloat4(&m_position), lookAt);
	XMStoreFloat4(&m_lookAt, lookAt);

	m_pitch = 0.0f;
	m_yaw = 0.0f;
}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(m_position.x, m_position.y, m_position.z);
}

XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(m_rotation.x, m_rotation.y, m_rotation.z);
}

XMFLOAT3 Camera::GetRight()
{
	return XMFLOAT3(m_right.x, m_right.y, m_right.z);
}

XMFLOAT3 Camera::GetLookAt()
{
	return XMFLOAT3(m_lookAt.x, m_lookAt.y, m_lookAt.z);
}

XMFLOAT3 Camera::GetUp()
{
	return XMFLOAT3(m_up.x, m_up.y, m_up.z);
}

void Camera::MoveForward()
{
	m_moveForward = true;
	m_moveBackward = false;
}

void Camera::MoveBackward()
{
	m_moveForward = false;
	m_moveBackward = true;
}

void Camera::MoverLeft()
{
	m_moveLeft = true;
	m_moveRight = false;
}

void Camera::MoverRight()
{
	m_moveRight = true;
	m_moveLeft = false;
}
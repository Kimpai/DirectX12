#include "cameraclass.h"

Camera::Camera(Input* inputHandler) : m_inputHandler(inputHandler)
{
	m_position = XMFLOAT3(0.0f, 0.0f, -2.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_forwardSpeed = 0.0f;
	m_backwardSpeed = 0.0f;
	m_rightSpeed = 0.0f;
	m_leftSpeed = 0.0f;
	m_turnSpeed = 0.0f;
	m_frameTime = 0.0f;
}

Camera::Camera(const Camera& other)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rotation = XMFLOAT3(x, y, z);
}

XMFLOAT3 Camera::GetPosition()
{
	return m_position;
}

XMFLOAT3 Camera::GetRotation()
{
	return m_rotation;
}

void Camera::Frame()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	//Setup the vector that points upwards
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//Load it into a XMVECTOR
	upVector = XMLoadFloat3(&up);

	//Setup the position of the camera
	position.x = m_position.x;
	position.y = m_position.y;
	position.z = m_position.z;

	//Load it into a XMVECTOR
	positionVector = XMLoadFloat3(&position);

	//Setup where the camera is looking
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//Load it into a XMVECTOR
	lookAtVector = XMLoadFloat3(&lookAt);

	//Set the yaw (Y axis), pitch (X axis) and roll (Z axis)
	//rotation in radians
	pitch = m_rotation.x * 0.0174532925f;
	yaw = m_rotation.y * 0.0174532925f;
	roll = m_rotation.z * 0.0174532925f;

	//Create the rotation matrix from yaw, pitch and roll
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//Transform the lookAt and up vector by the rotation matrix
	//so the view is correctly rotated at the origin
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//Translate the rotated camera position to the location of the viewer
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//Finally cretae the view matrix from the updated vectors
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void Camera::BuildBaseViewMatrix()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	//Setup the vector that points upwards
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//Load it into a XMVECTOR
	upVector = XMLoadFloat3(&up);

	//Setup the position of the camera
	position.x = m_position.x;
	position.y = m_position.y;
	position.z = m_position.z;

	//Load it into a XMVECTOR
	positionVector = XMLoadFloat3(&position);

	//Setup where the camera is looking
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//Load it into a XMVECTOR
	lookAtVector = XMLoadFloat3(&lookAt);

	//Set the yaw (Y axis), pitch (X axis) and roll (Z axis)
	//rotation in radians
	pitch = m_rotation.x * 0.0174532925f;
	yaw = m_rotation.y * 0.0174532925f;
	roll = m_rotation.z * 0.0174532925f;

	//Create the rotation matrix from yaw, pitch and roll
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//Transform the lookAt and up vector by the rotation matrix
	//so the view is correctly rotated at the origin
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//Translate the rotated camera position to the location of the viewer
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//Finally cretae the view matrix from the updated vectors
	m_baseViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void Camera::GetBaseViewMatrix(XMMATRIX& baseViewMatrix)
{
	baseViewMatrix = m_baseViewMatrix;
}

void Camera::MoveForward()
{
	float radians;

	//Update forward speed
	if (m_inputHandler->IsKeyDown(VK_W))
	{
		m_forwardSpeed += m_frameTime * 1.0f;
		if (m_forwardSpeed > (m_frameTime * 50.0f))
			m_forwardSpeed = m_frameTime * 50.0f;
	}
	else
	{
		m_forwardSpeed -= m_frameTime * 0.5f;
		if (m_forwardSpeed < 0.0f)
			m_forwardSpeed = 0.0f;
	}

	//Convert degrees to radians
	radians = m_rotation.y * 0.0174532925f;

	//Update the position
	m_position.x += sinf(radians) * m_forwardSpeed;
	m_position.z += cosf(radians) * m_forwardSpeed;
}

void Camera::MoveBackward()
{
	float radians;

	if (m_inputHandler->IsKeyDown(VK_S))
	{
		m_backwardSpeed += m_frameTime * 1.0f;

		if (m_backwardSpeed > (m_frameTime * 50.0f))
		{
			m_backwardSpeed = m_frameTime * 50.0f;
		}
	}
	else
	{
		m_backwardSpeed -= m_frameTime * 0.5f;

		if (m_backwardSpeed < 0.0f)
		{
			m_backwardSpeed = 0.0f;
		}
	}

	radians = m_rotation.y * 0.0174532925f;

	m_position.x -= sinf(radians) * m_backwardSpeed;
	m_position.z -= cosf(radians) * m_backwardSpeed;
}

void Camera::MoveRight()
{
}

void Camera::MoveLeft()
{
}

void Camera::Turn()
{
}

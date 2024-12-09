/*
	Author: Annabelle Thompson
*/
#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) 
{
	mPosition = position;
	mWorldUp = up;
	mYaw = yaw;
	mPitch = pitch;
	updateCameraVectors();

	mFront = glm::vec3(0.0f, 0.0f, -1.0f);
	mMovementSpeed = SPEED;
	mMouseSens = SENS;
	mZoom = ZOOM;
}

Camera::Camera(float xPos, float yPos, float zPos, float xUp, float yUp, float zUp, float yaw, float pitch)
{
	mPosition = glm::vec3(xPos, yPos, zPos);
	mWorldUp = glm::vec3(xUp, yUp, zUp);
	mYaw = yaw;
	mPitch = pitch;
	updateCameraVectors();
	
	mFront = glm::vec3(0.0f, 0.0f, -1.0f);
	mMovementSpeed = SPEED;
	mMouseSens = SENS;
	mZoom = ZOOM;
}

glm::vec3 Camera::getPos()
{
	return mPosition;
}

glm::mat4 Camera::getViewMatrix()
{
	return lookAt(mPosition, mPosition + mFront, mUp);
}

void Camera::keyboardInput(CameraMovement direction, float deltaTime, bool sprit)
{
	float velocity = mMovementSpeed * deltaTime;

	switch (direction)
	{
	case FORWARD:
		mPosition += mFront * velocity;
		break;
	case BACKWARD:
		mPosition -= mFront * velocity;
		break;
	case LEFT:
		mPosition -= mRight * velocity;
		break;
	case RIGHT:
		mPosition += mRight * velocity;
		break;
	case UP:
		mPosition += mUp * velocity;
		break;
	case DOWN:
		mPosition -= mUp * velocity;
		break;
	case SPRINT:
		mMovementSpeed = SPEED * 2;
		break;
	case WALK:
		mMovementSpeed = SPEED;
		break;
	}
}

void Camera::mouseMoveInput(float xOffset, float yOffset, GLboolean constrainPitch)
{
	xOffset *= mMouseSens;
	yOffset *= mMouseSens;

	mYaw += xOffset;
	mPitch += yOffset;

	if (constrainPitch)
	{
		if (mPitch > 89.0f)
		{
			mPitch = 89.0f;
		}
		if (mPitch < -89.0)
		{
			mPitch = -89.0;
		}
	}

	updateCameraVectors();
}

void Camera::mouseWheelInput(float yOffset)
{
	mZoom -= (float)yOffset;
	if (mZoom < 1.0f)
	{
		mZoom = 1.0f;
	}
	if (mZoom > 120.0f)
	{
		mZoom = 120.0f;
	}
}

void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	front.y = sin(glm::radians(-mPitch));
	front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	mFront = glm::normalize(front);

	mRight = glm::normalize(glm::cross(mFront, mWorldUp));
	mUp = glm::normalize(glm::cross(mRight, mFront));
}

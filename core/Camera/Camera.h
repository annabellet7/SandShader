/*
	Author: Annabelle Thompson
*/
#ifndef CAMERA_H
#define CAMERA_H

#include "..\ew\external\glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

enum CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	SPRINT,
	WALK
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 15.0f;
const float SENS = 0.1f;
const float ZOOM = 60.0f;

class Camera
{
public:
	glm::vec3 mPosition, mFront, mUp, mRight, mWorldUp; // cam attrib
	float mYaw, mPitch; // angles
	float mMovementSpeed, mMouseSens, mZoom; // cam options

	//constructors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	Camera(float xPos, float yPos, float zPos, float upX, float upY, float upZ, float yaw, float pitch);

	glm::vec3 getPos();
	glm::mat4 getViewMatrix();
	void keyboardInput(CameraMovement direction, float deltaTime, bool sprint = false);
	void mouseMoveInput(float xOffset, float yOffset, GLboolean constrainPitch = true);
	void mouseWheelInput(float yOffset);

private:
	void updateCameraVectors();
};

#endif
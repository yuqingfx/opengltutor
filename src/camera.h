#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.f;

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp = glm::vec3(0.f, 1.f, 0.f);

	// Euler angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;


	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.f, 0.f, 0.f))
		: Front(glm::vec3(0.f, 0.f, -1.f))
		, Position(position)
		, Yaw(YAW)
		, Pitch(PITCH)
		, MovementSpeed(SPEED)
		, MouseSensitivity(SENSITIVITY)
		, Zoom(ZOOM)
	{
		UpdateCameraVectors();
	}

	// Constructor with scalar values
	Camera( float posX, float posY, float posZ,
			float yaw, float pitch)
			: Front(glm::vec3(0.0f, 0.0f, -1.0f))
			, Position(glm::vec3(posX, posY, posZ))
			, Yaw(YAW)
			, Pitch(PITCH)
			, MovementSpeed(SPEED)
			, MouseSensitivity(SENSITIVITY)
			, Zoom(ZOOM)
	{
		UpdateCameraVectors();
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constraintPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw   += xoffset;
		Pitch += yoffset;

		if (constraintPitch)
		{
			if (Pitch > 89.f)
				Pitch = 89.0f;
			if (Pitch < -89.f)
				Pitch = -89.f;
		}

		UpdateCameraVectors();
	}
	
	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.f)
			Zoom = 45.f;
	}

private:
	void UpdateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));

	}
};

#endif
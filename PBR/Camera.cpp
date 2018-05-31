#include "Camera.h"

CCamera::CCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
	c_vFront(glm::vec3(0.0f, 0.0f, -1.0f)), c_fMovementSpeed(_cSPEED), c_fMouseSensitivity(_cSENSITIVTY), c_fZoom(_cZOOM)
{
	c_vPosition = position;
	c_vWorldUp = up;
	c_fYaw = yaw;
	c_fPitch = pitch;
	UpdateCameraVectors();
}

// Constructor with scalar values
CCamera::CCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : 
	c_vFront(glm::vec3(0.0f, 0.0f, -1.0f)), c_fMovementSpeed(_cSPEED), c_fMouseSensitivity(_cSENSITIVTY), c_fZoom(_cZOOM)
{
	c_vPosition = glm::vec3(posX, posY, posZ);
	c_vWorldUp = glm::vec3(upX, upY, upZ);
	c_fYaw = yaw;
	c_fPitch = pitch;
	UpdateCameraVectors();
}

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 CCamera::GetViewMatrix()
{
	return glm::lookAt(c_vPosition, c_vPosition + c_vFront, c_vUp);
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void CCamera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
	float fVelocity = c_fMovementSpeed * deltaTime;
	switch (direction) {
	case CM_FORWARD:
		c_vPosition += c_vFront * fVelocity;
		break;
	case CM_BACKWARD:
		c_vPosition -= c_vFront * fVelocity;
		break;
	case CM_LEFT:
		c_vPosition -= c_vRight * fVelocity;
		break;
	case CM_RIGHT:
		c_vPosition += c_vRight * fVelocity;
		break;
	case CM_UP:
		c_vPosition += glm::vec3(0.0f, 1.0f, 0.0f) * fVelocity;
		break;
	case CM_DOWN:
		c_vPosition -= glm::vec3(0.0f, 1.0f, 0.0f) * fVelocity;
		break;
	}
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void CCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= c_fMouseSensitivity;
	yoffset *= c_fMouseSensitivity;

	c_fYaw += xoffset;
	c_fPitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch) {
		if (c_fPitch > 89.0f) {
			c_fPitch = 89.0f;
		}
		if (c_fPitch < -89.0f) {
			c_fPitch = -89.0f;
		}
	}

	// Update Front, Right and Up Vectors using the updated Eular angles
	UpdateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void CCamera::ProcessMouseScroll(float yoffset)
{
	if (c_fZoom >= 1.0f && c_fZoom <= 45.0f) {
		c_fZoom -= yoffset;
	}
	if (c_fZoom <= 1.0f) {
		c_fZoom = 1.0f;
	}
	if (c_fZoom >= 45.0f) {
		c_fZoom = 45.0f;
	}
}

// Calculates the front vector from the Camera's (updated) Eular Angles
void CCamera::UpdateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(c_fYaw)) * cos(glm::radians(c_fPitch));
	front.y = sin(glm::radians(c_fPitch));
	front.z = sin(glm::radians(c_fYaw)) * cos(glm::radians(c_fPitch));
	c_vFront = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	c_vRight = glm::normalize(glm::cross(c_vFront, c_vWorldUp));
	c_vUp = glm::normalize(glm::cross(c_vRight, c_vFront));
}
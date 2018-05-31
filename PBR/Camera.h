#ifndef PBR_CAMERA_H
#define PBR_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement {
	CM_FORWARD,
	CM_BACKWARD,
	CM_LEFT,
	CM_RIGHT,
	CM_UP,
	CM_DOWN,
};

const float _cYAW = -90.0f;
const float _cPITCH = 0.0f;
const float _cSPEED = 12.5f;
const float _cSENSITIVTY = 0.1f;
const float _cZOOM = 75.0f;

class CCamera {
public:
	glm::vec3 c_vPosition;
	glm::vec3 c_vFront;
	glm::vec3 c_vUp;
	glm::vec3 c_vRight;
	glm::vec3 c_vWorldUp;
	// Eular Angles
	float c_fYaw;
	float c_fPitch;
	// Camera options
	float c_fMovementSpeed;
	float c_fMouseSensitivity;
	float c_fZoom;
public:
	CCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = _cYAW, float pitch = _cPITCH);
	CCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
	glm::mat4 GetViewMatrix();
	void ProcessKeyboard(CameraMovement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
	void ProcessMouseScroll(float yoffset);

private:
	void UpdateCameraVectors();
};

#endif // PBR_CAMERA_H
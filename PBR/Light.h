#ifndef PBR_LIGHT_H
#define PBR_LIGHT_H

#include <glm/glm.hpp>
#include "Shader.h"

class CLight {
public:
	glm::vec3 l_vPosition;
	glm::vec3 l_vColor;
public:
	CLight(glm::vec3 vPos, glm::vec3 vColor);
	~CLight();
	void Prepare(CShader &shShader, int iIndex);
	void Render(CShader &shShader);
};

#endif // PBR_LIGHT_H
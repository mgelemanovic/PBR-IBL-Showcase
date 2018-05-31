#include "Light.h"

#include <glm/gtc/matrix_transform.hpp>
#include "GeometryHelper.h"

CLight::CLight(glm::vec3 vPos, glm::vec3 vColor)
{
	l_vPosition = vPos;
	l_vColor = vColor;
}

CLight::~CLight()
{
}

void CLight::Prepare(CShader &shShader, int iIndex)
{
	shShader.SetVec3("lightPositions[" + std::to_string(iIndex) + "]", l_vPosition);
	shShader.SetVec3("lightColors[" + std::to_string(iIndex) + "]", l_vColor);
}

void CLight::Render(CShader &shShader)
{
	glm::mat4 mModel = glm::mat4();
	mModel = glm::translate(mModel, l_vPosition);
	mModel = glm::scale(mModel, glm::vec3(0.5f));
	shShader.SetMat4("model", mModel);

	_ghRenderSphere();
}

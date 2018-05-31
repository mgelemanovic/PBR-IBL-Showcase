#include "Renderable.h"

#include <glm/gtc/matrix_transform.hpp>
#include "GeometryHelper.h"
#include "Shader.h"
#include "Texture.h"

CRenderable::CRenderable()
{
	re_vPosition = glm::vec3(0.0f);
	re_vRotation = glm::vec3(0.0f);
	re_vScale = glm::vec3(1.0f);
	re_vAlbedo = glm::vec3(1.0f);
	re_fMetallic = 0.5f;
	re_fRoughness = 0.5f;
	re_fAO = 1.0f;
	re_ptexPBR = nullptr;
}

CRenderable::~CRenderable()
{
}

void CRenderable::SetPosition(glm::vec3 vPos)
{
	re_vPosition = vPos;
}

void CRenderable::SetScale(glm::vec3 vScale)
{
	re_vScale = vScale;
}

void CRenderable::SetRotation(glm::vec3 vRotation)
{
	re_vRotation = vRotation;
}

void CRenderable::SetColor(glm::vec3 vCol)
{
	re_vAlbedo = vCol;
}

void CRenderable::SetRoughness(float fRoughness)
{
	re_fRoughness = fRoughness;
}

void CRenderable::SetMetallic(float fMetallic)
{
	re_fMetallic = fMetallic;
}

void CRenderable::SetAO(float fAO)
{
	re_fAO = fAO;
}

void CRenderable::SetTexture(CTexture *ptex)
{
	re_ptexPBR = ptex;
}

void CRenderable::PreRender(CShader *psh)
{
	glm::mat4 mModel = glm::mat4();
	mModel = glm::translate(mModel, re_vPosition);
	mModel = glm::rotate(mModel, glm::radians(re_vRotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
	mModel = glm::rotate(mModel, glm::radians(re_vRotation.y), glm::vec3(-1.0f, 0.0f, 0.0f));
	mModel = glm::scale(mModel, re_vScale);
	psh->SetMat4("model", mModel);
}

void CRenderable::Render(CShader *psh)
{
	PreRender(psh);

	bool bIsTextured = re_ptexPBR != nullptr;
	psh->SetBool("isTextured", bIsTextured);
	if (bIsTextured) {
		re_ptexPBR->Render();
	} else {
		psh->SetFloat("metallicValue", re_fMetallic);
		psh->SetFloat("roughnessValue", re_fRoughness);
		psh->SetVec3("albedoValue", re_vAlbedo);
		psh->SetFloat("aoValue", re_fAO);
	}

	_ghRenderSphere();
}

#ifndef PBR_RENDERABLE_H
#define PBR_RENDERABLE_H

#include <glm/glm.hpp>

class CTexture;
class CShader;

class CRenderable {
protected:
	glm::vec3 re_vPosition;
	glm::vec3 re_vScale;
	glm::vec3 re_vRotation;
	glm::vec3 re_vAlbedo;
	float re_fRoughness;
	float re_fMetallic;
	float re_fAO;
	CTexture *re_ptexPBR;
public:
	CRenderable();
	~CRenderable();
	void SetPosition(glm::vec3 vPos);
	void SetScale(glm::vec3 vScale);
	void SetRotation(glm::vec3 vRotation);
	void SetColor(glm::vec3 vCol);
	void SetRoughness(float fRoughness);
	void SetMetallic(float fMetallic);
	void SetAO(float fAO);
	void SetTexture(CTexture *ptex);
	void PreRender(CShader *psh);
	virtual void Render(CShader *psh);
};

#endif // PBR_RENDERABLE_H
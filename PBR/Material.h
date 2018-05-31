#ifndef PBR_MATERIAL_H
#define PBR_MATERIAL_H

#include <glm/glm.hpp>

class CShader;
class aiMaterial;

class CMaterial {
private:
	glm::vec3 mat_vDiffuse;
	glm::vec3 mat_vAmbient;
	glm::vec3 mat_vSpecular;
	float mat_fAlpha;
public:
	CMaterial();
	void Init(aiMaterial *pasMaterial);
	void Render(CShader *psh);
	bool IsTransparent(void);
};

#endif
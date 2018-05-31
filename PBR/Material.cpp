#include "Material.h"

#include "Shader.h"
#include <assimp/scene.h>

CMaterial::CMaterial() :
	mat_vDiffuse(0.0f), mat_vAmbient(0.0f), mat_vSpecular(0.0f), mat_fAlpha(1.0f)
{
}

void CMaterial::Init(aiMaterial *pasMaterial)
{
	aiColor3D vColor(0.f, 0.f, 0.f);
	if (pasMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, vColor) == AI_SUCCESS) {
		mat_vDiffuse = glm::vec3(vColor.r, vColor.g, vColor.b);
	}

	if (pasMaterial->Get(AI_MATKEY_COLOR_AMBIENT, vColor) == AI_SUCCESS) {
		mat_vAmbient = glm::vec3(vColor.r, vColor.g, vColor.b);
	}

	if (pasMaterial->Get(AI_MATKEY_COLOR_SPECULAR, vColor) == AI_SUCCESS) {
		mat_vSpecular = glm::vec3(vColor.r, vColor.g, vColor.b);
	}

	float fAlpha = 1.0f;
	if (pasMaterial->Get(AI_MATKEY_OPACITY, fAlpha) == AI_SUCCESS) {
		mat_fAlpha = fAlpha;
	}
}

void CMaterial::Render(CShader *psh)
{
	psh->SetVec3("difuseColor", mat_vDiffuse);
	psh->SetVec3("ambientColor", mat_vAmbient);
	psh->SetVec3("specularColor", mat_vSpecular);
	psh->SetFloat("alpha", mat_fAlpha);
	psh->SetVec3("albedoValue", mat_vDiffuse);
	psh->SetFloat("metallicValue", 0.99f);
	psh->SetFloat("roughnessValue", 0.99f);
	psh->SetFloat("aoValue", 1.0f);
}

bool CMaterial::IsTransparent(void)
{
	return mat_fAlpha < 1.0f;
}

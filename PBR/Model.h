#ifndef PBR_MODEL_H
#define PBR_MODEL_H

#include "Mesh.h"
#include <assimp/scene.h>

class CTexture;

class CModelRenderable {
public:
	// placement data
	glm::vec3 re_vPosition;
	glm::vec3 re_vScale;
	glm::vec3 re_vRotation;
	// PBR material
	CTexture *re_ptexPBR;
	// mesh data
	std::vector<CMesh*> re_ameMeshes;
	std::vector<int> re_aiSkippedMeshes;
	std::string re_strName;
public:
	CModelRenderable(std::string &strPath);
	void SkipMesh(int iIndex);
	void LoadModel(std::string &strPath);
	void ProcessNode(aiNode *pasNode, const aiScene *pasScene);
	CMesh *ProcessMesh(aiMesh *pasMesh, const aiScene *pasScene);
	void Render(CShader *psh);
};

#endif // PBR_MODEL_H
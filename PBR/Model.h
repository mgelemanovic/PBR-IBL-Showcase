#ifndef PBR_MODEL_H
#define PBR_MODEL_H

#include "Renderable.h"
#include "Mesh.h"
#include <assimp/scene.h>

class CModelRenderable : public CRenderable {
public:
	std::vector<CMesh*> re_ameMeshes;
	std::vector<Texture> re_atexTextures;
	std::vector<int> re_aiSkippedMeshes;
	std::string re_strDirectory;
public:
	CModelRenderable(std::string &strPath);
	void SkipMesh(int iIndex);
	void LoadModel(std::string &strPath);
	void SetPBRTexture(CTexture *ptex);
	void ProcessNode(aiNode *pasNode, const aiScene *pasScene);
	CMesh *ProcessMesh(aiMesh *pasMesh, const aiScene *pasScene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial *pasMaterial, aiTextureType asType);
	virtual void Render(CShader *psh);
};

#endif // PBR_MODEL_H
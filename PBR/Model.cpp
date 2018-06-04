#include "Model.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "GeometryHelper.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <iostream>

#include "GeometryHelper.h"

CModelRenderable::CModelRenderable(std::string &strPath)
{
	re_aiSkippedMeshes.push_back(-1);
	if (!strPath.empty()) {
		LoadModel(strPath);
	}
}

void CModelRenderable::SkipMesh(int iIndex)
{
	re_aiSkippedMeshes.push_back(iIndex);
}

void CModelRenderable::LoadModel(std::string &strPath)
{
	std::cout << "LOG: Loading model @" << strPath << std::endl;
	// read model from file
	Assimp::Importer asImporter;
	const aiScene *pasScene = asImporter.ReadFile(strPath, aiProcess_Triangulate | 
		aiProcess_JoinIdenticalVertices | aiProcess_GenNormals);
	
	// check for errors
	if (!pasScene || pasScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pasScene->mRootNode) {
		std::cout << "ERR: " << asImporter.GetErrorString() << std::endl;
		return;
	}

	ProcessNode(pasScene->mRootNode, pasScene);
}

void CModelRenderable::ProcessNode(aiNode *pasNode, const aiScene *pasScene)
{
	// process all meshes in this node
	for (GLuint i = 0; i < pasNode->mNumMeshes; ++i) {
		aiMesh *pasMesh = pasScene->mMeshes[pasNode->mMeshes[i]];
		re_ameMeshes.push_back(ProcessMesh(pasMesh, pasScene));
	}
	// process meshes in children nodes
	for (GLuint i = 0; i < pasNode->mNumChildren; ++i) {
		ProcessNode(pasNode->mChildren[i], pasScene);
	}
}

CMesh *CModelRenderable::ProcessMesh(aiMesh *pasMesh, const aiScene *pasScene)
{
	// data to fill
	std::vector<Vertex> avtVertices;
	std::vector<unsigned int> aiIndices;

	// walk through each of the mesh's vertices
	for (GLuint i = 0; i < pasMesh->mNumVertices; ++i) {
		// we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		Vertex vtVertex;
		glm::vec3 vTmpVector;
		// positions
		vTmpVector.x = pasMesh->mVertices[i].x;
		vTmpVector.y = pasMesh->mVertices[i].y;
		vTmpVector.z = pasMesh->mVertices[i].z;
		vtVertex.vt_vPosition = vTmpVector;
		// normals
		if (pasMesh->mNormals) {
			vTmpVector.x = pasMesh->mNormals[i].x;
			vTmpVector.y = pasMesh->mNormals[i].y;
			vTmpVector.z = pasMesh->mNormals[i].z;
			vtVertex.vt_vNormal = vTmpVector;
		}

		// does the mesh contain texture coordinates?
		if (pasMesh->mTextureCoords[0]) {
			glm::vec2 vTmpVector;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vTmpVector.x = pasMesh->mTextureCoords[0][i].x;
			vTmpVector.y = pasMesh->mTextureCoords[0][i].y;
			vtVertex.vt_vTextureCoords = vTmpVector;
		} else {
			vtVertex.vt_vTextureCoords = glm::vec2(0.0f, 0.0f);
		}
		avtVertices.push_back(vtVertex);
	}
	// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (GLuint i = 0; i < pasMesh->mNumFaces; i++) {
		aiFace asFace = pasMesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < asFace.mNumIndices; j++) {
			aiIndices.push_back(asFace.mIndices[j]);
		}
	}

	// return a mesh object created from the extracted mesh data
	return new CMesh(avtVertices, aiIndices);
}

void CModelRenderable::Render(CShader *psh)
{
	if (re_ptexPBR == NULL) {
		return;
	}

	glm::mat4 mModel = glm::mat4();
	mModel = glm::translate(mModel, re_vPosition);
	mModel = glm::rotate(mModel, glm::radians(re_vRotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
	mModel = glm::rotate(mModel, glm::radians(re_vRotation.y), glm::vec3(-1.0f, 0.0f, 0.0f));
	mModel = glm::scale(mModel, re_vScale);
	psh->SetMat4("model", mModel);

	re_ptexPBR->Render();

	// render sphere if there are no meshes loaded
	int iMeshCount = re_ameMeshes.size();
	if (iMeshCount == 0) {
		_ghRenderSphere();
		return;
	}

	int iSkipCount = re_aiSkippedMeshes.size();
	int iSkip = iSkipCount > 1 ? 1 : 0;
	// render every mesh
	for (GLuint i = 0; i < iMeshCount; ++i) {
		if (i != re_aiSkippedMeshes[iSkip]) {
			re_ameMeshes[i]->Render(psh);
		} else {
			iSkip = (iSkip + 1) % iSkipCount;
		}
	}
}
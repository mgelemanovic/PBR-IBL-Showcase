#include "Model.h"
#include "Material.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <iostream>

#include "GeometryHelper.h"

CModelRenderable::CModelRenderable(std::string &strPath)
{
	re_aiSkippedMeshes.push_back(-1);
	LoadModel(strPath);
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
	// remember model directory path
	re_strDirectory = strPath.substr(0, strPath.find_last_of("\\"));

	ProcessNode(pasScene->mRootNode, pasScene);
}

void CModelRenderable::SetPBRTexture(CTexture * ptex)
{
	re_ptexPBR = ptex;
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
	std::vector<Texture> atexTextures;

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
	// process materials
	aiMaterial *pasMaterial = pasScene->mMaterials[pasMesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	std::vector<Texture> diffuseMaps = LoadMaterialTextures(pasMaterial, aiTextureType_DIFFUSE);
	atexTextures.insert(atexTextures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture> specularMaps = LoadMaterialTextures(pasMaterial, aiTextureType_SPECULAR);
	atexTextures.insert(atexTextures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = LoadMaterialTextures(pasMaterial, aiTextureType_HEIGHT);
	atexTextures.insert(atexTextures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = LoadMaterialTextures(pasMaterial, aiTextureType_AMBIENT);
	atexTextures.insert(atexTextures.end(), heightMaps.begin(), heightMaps.end());
	// 5. emissive maps
	std::vector<Texture> emissiveMaps = LoadMaterialTextures(pasMaterial, aiTextureType_EMISSIVE);
	atexTextures.insert(atexTextures.end(), emissiveMaps.begin(), emissiveMaps.end());

	CMaterial *pmat = new CMaterial();
	pmat->Init(pasMaterial);

	// return a mesh object created from the extracted mesh data
	return new CMesh(avtVertices, aiIndices, atexTextures, pmat);
}

std::vector<Texture> CModelRenderable::LoadMaterialTextures(aiMaterial *pasMaterial, aiTextureType asType)
{
	std::vector<Texture> atexTextures;
	for (unsigned int i = 0; i < pasMaterial->GetTextureCount(asType); i++) {
		aiString str;
		pasMaterial->GetTexture(asType, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < re_atexTextures.size(); ++j) {
			if (std::strcmp(re_atexTextures[j].tex_asPath.C_Str(), str.C_Str()) == 0) {
				atexTextures.push_back(re_atexTextures[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		// if texture hasn't been loaded already, load it
		if (!skip) {
			Texture texTexture;
			texTexture.tex_iID = _texLoadTextureFromFile(str.C_Str(), re_strDirectory.c_str());
			texTexture.tex_asType = asType;
			texTexture.tex_asPath = str;
			atexTextures.push_back(texTexture);
			re_atexTextures.push_back(texTexture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return atexTextures;
}

void CModelRenderable::Render(CShader *psh)
{
	PreRender(psh);

	bool bIsTextured = re_ptexPBR != nullptr;
	psh->SetBool("isTextured", bIsTextured);
	if (bIsTextured) {
		re_ptexPBR->Render();
	}

	int iSkipCount = re_aiSkippedMeshes.size();
	int iSkip = iSkipCount > 1 ? 1 : 0;
	// render every mesh
	for (GLuint i = 0; i < re_ameMeshes.size(); ++i) {
		if (i != re_aiSkippedMeshes[iSkip]) {
			re_ameMeshes[i]->Render(psh, !bIsTextured);
		} else {
			iSkip = (iSkip + 1) % iSkipCount;
		}
	}
}
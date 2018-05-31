#include "Mesh.h"
#include "Shader.h"
#include "Material.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

CMesh::CMesh(std::vector<Vertex> &avtVertices, std::vector<GLuint> &aiIndices, std::vector<Texture> &atexTextures, CMaterial *pmat) :
	mesh_avtVertices(avtVertices), mesh_aiIndices(aiIndices), mesh_atexTextures(atexTextures), mesh_pmatMaterial(pmat)
{
	Initialize();
	//CreateBoundingBox();
}

CMesh::~CMesh(void)
{
	delete(mesh_pmatMaterial);
}

void CMesh::Initialize(void)
{
	glGenVertexArrays(1, &mesh_iVAO);
	glGenBuffers(1, &mesh_iVBO);
	glGenBuffers(1, &mesh_iEBO);

	glBindVertexArray(mesh_iVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_iVBO);

	glBufferData(GL_ARRAY_BUFFER, mesh_avtVertices.size() * sizeof(Vertex), &mesh_avtVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_iEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_aiIndices.size() * sizeof(unsigned int), &mesh_aiIndices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vt_vNormal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vt_vTextureCoords));

	glBindVertexArray(0);
}

void CMesh::CreateBoundingBox(void)
{
	mesh_bbBoundingBox.bb_vMaxValues = mesh_bbBoundingBox.bb_vMaxValues = mesh_avtVertices[0].vt_vPosition;
	for (int iIndex = 1; iIndex < mesh_avtVertices.size(); ++iIndex) {
		const glm::vec3 &vCurrentVertex = mesh_avtVertices[iIndex].vt_vPosition;
		// max values
		if (mesh_bbBoundingBox.bb_vMaxValues.x < vCurrentVertex.x) {
			mesh_bbBoundingBox.bb_vMaxValues.x = vCurrentVertex.x;
		}
		if (mesh_bbBoundingBox.bb_vMaxValues.y < vCurrentVertex.y) {
			mesh_bbBoundingBox.bb_vMaxValues.y = vCurrentVertex.y;
		}
		if (mesh_bbBoundingBox.bb_vMaxValues.z < vCurrentVertex.z) {
			mesh_bbBoundingBox.bb_vMaxValues.z = vCurrentVertex.z;
		}
		// min values
		if (mesh_bbBoundingBox.bb_vMinValues.x > vCurrentVertex.x) {
			mesh_bbBoundingBox.bb_vMinValues.x = vCurrentVertex.x;
		}
		if (mesh_bbBoundingBox.bb_vMinValues.y > vCurrentVertex.y) {
			mesh_bbBoundingBox.bb_vMinValues.y = vCurrentVertex.y;
		}
		if (mesh_bbBoundingBox.bb_vMinValues.z > vCurrentVertex.z) {
			mesh_bbBoundingBox.bb_vMinValues.z = vCurrentVertex.z;
		}
	}
	mesh_bbBoundingBox.bb_vCenter = (mesh_bbBoundingBox.bb_vMaxValues + mesh_bbBoundingBox.bb_vMinValues) / 2.0f;
}

void CMesh::Render(CShader *psh, bool bUseTextures)
{
	GLuint bUsingEmissiveTexture = 0;
	if (bUseTextures) {
		// bind appropriate textures
		GLuint iDiffuseNr = 1;
		GLuint iSpecularNr = 1;
		//GLuint iNormalNr = 1;
		//GLuint iHeightNr = 1;
		GLuint iTexturesCount = mesh_atexTextures.size();
		if (iTexturesCount == 0) {
			glActiveTexture(GL_TEXTURE0);
			psh->SetInt("texture_diffuse1", 0);
			glBindTexture(GL_TEXTURE_2D, mesh_iEmptyTexture);

		}
		for (GLuint i = 0; i < iTexturesCount; ++i) {
			// active proper texture unit before binding
			glActiveTexture(GL_TEXTURE0 + i);

			// retrieve texture number (the N in diffuse_textureN)
			std::string strTextureLocation;
			switch (mesh_atexTextures[i].tex_asType) {
			case aiTextureType_DIFFUSE:
				strTextureLocation = "texture_diffuse" + std::to_string(iDiffuseNr++);
				break;
			case aiTextureType_SPECULAR:
				strTextureLocation = "texture_specular" + std::to_string(iSpecularNr++);
				break;
			case aiTextureType_EMISSIVE:
				strTextureLocation = "texture_diffuse" + std::to_string(iDiffuseNr++);
				bUsingEmissiveTexture = 1;
				break;
			}

			// now set the sampler to the correct texture unit
			psh->SetInt(strTextureLocation.c_str(), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, mesh_atexTextures[i].tex_iID);
		}
	}
	psh->SetInt("emissive_texture", bUsingEmissiveTexture);

	mesh_pmatMaterial->Render(psh);

	// draw mesh
	glBindVertexArray(mesh_iVAO);
	glDrawElements(GL_TRIANGLES, mesh_aiIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}


GLuint CMesh::mesh_iEmptyTexture = -1;
GLuint CMesh::mesh_iEmptyEmissive = -1;

void CMesh::GenerateEmptyTexture(void)
{
	glGenTextures(1, &mesh_iEmptyTexture);
	glBindTexture(GL_TEXTURE_2D, mesh_iEmptyTexture);
	GLubyte texData[] = { 255, 255, 255, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);

	glGenTextures(1, &mesh_iEmptyEmissive);
	glBindTexture(GL_TEXTURE_2D, mesh_iEmptyEmissive);
	GLubyte texData2[] = { 0, 0, 0, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData2);
}
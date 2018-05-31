#ifndef PBR_MESH_H
#define PBR_MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <string>
#include <vector>

class CShader;
class CMaterial;

struct Vertex {
	glm::vec3 vt_vPosition;
	glm::vec3 vt_vNormal;
	glm::vec2 vt_vTextureCoords;
};

enum TextureType {
	TT_DIFFUSE,
	TT_SPECULAR,
};

struct Texture {
	GLuint tex_iID;
	aiTextureType tex_asType;
	aiString tex_asPath;
};

struct BoundingBox {
	glm::vec3 bb_vMinValues;
	glm::vec3 bb_vMaxValues;
	glm::vec3 bb_vCenter;
};

class CMesh {
private:
	GLuint mesh_iVAO;
	GLuint mesh_iVBO;
	GLuint mesh_iEBO;
public:
	std::vector<Vertex> mesh_avtVertices;
	std::vector<GLuint> mesh_aiIndices;
	std::vector<Texture> mesh_atexTextures;
	CMaterial *mesh_pmatMaterial;
	BoundingBox mesh_bbBoundingBox;
private:
	static GLuint mesh_iEmptyTexture;
	static GLuint mesh_iEmptyEmissive;
public:
	static void GenerateEmptyTexture(void);
public:
	CMesh(std::vector<Vertex> &avtVertices, std::vector<GLuint> &aiIndices, std::vector<Texture> &atexTextures, CMaterial *pmat);
	~CMesh(void);
	void Initialize(void);
	void CreateBoundingBox(void);
	void Render(CShader *psh, bool bUseTextures);
};

#endif // PBR_MESH_H
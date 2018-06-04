#ifndef PBR_MESH_H
#define PBR_MESH_H

#include <glm/glm.hpp>
#include <vector>

class CShader;

struct Vertex {
	glm::vec3 vt_vPosition;
	glm::vec3 vt_vNormal;
	glm::vec2 vt_vTextureCoords;
};

class CMesh {
private:
	unsigned int mesh_iVAO;
	unsigned int mesh_iVBO;
	unsigned int mesh_iEBO;
public:
	std::vector<Vertex> mesh_avtVertices;
	std::vector<unsigned int> mesh_aiIndices;
public:
	CMesh(std::vector<Vertex> &avtVertices, std::vector<unsigned int> &aiIndices);
	~CMesh(void);
	void Render(CShader *psh);
};

#endif // PBR_MESH_H
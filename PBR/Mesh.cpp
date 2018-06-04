#include "Mesh.h"
#include "Shader.h"

#include <glad/glad.h>

CMesh::CMesh(std::vector<Vertex> &avtVertices, std::vector<GLuint> &aiIndices) :
	mesh_avtVertices(avtVertices), mesh_aiIndices(aiIndices)
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

CMesh::~CMesh(void)
{
}

void CMesh::Render(CShader *psh)
{
	// draw mesh
	glBindVertexArray(mesh_iVAO);
	glDrawElements(GL_TRIANGLES, mesh_aiIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
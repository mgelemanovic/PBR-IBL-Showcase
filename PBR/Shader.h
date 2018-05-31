#ifndef PBR_SHADER_H
#define PBR_SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class CShader {
public:
	GLuint sh_iProgram;
public:
	CShader(const char *strVertexFilePath, const char *strFragmentFilePath);
	~CShader(void);

	void Use(void);
	void SetBool(const std::string &strName, bool bValue);
	void SetInt(const std::string &strName, int iValue);
	void SetFloat(const std::string &strName, float fValue);
	void SetVec2(const std::string &strName, glm::vec2 vValue);
	void SetVec2(const std::string &strName, float fValueX, float fValueY);
	void SetVec3(const std::string &strName, glm::vec3 vValue);
	void SetVec3(const std::string &strName, float fValueX, float fValueY, float fValueZ);
	void SetVec4(const std::string &strName, glm::vec4 vValue);
	void SetVec4(const std::string &strName, float fValueX, float fValueY, float fValueZ, float fValueW);
	void SetMat2(const std::string &strName, glm::mat2 mValue);
	void SetMat3(const std::string &strName, glm::mat3 mValue);
	void SetMat4(const std::string &strName, glm::mat4 mValue);
};

#endif // PBR_SHADER_H
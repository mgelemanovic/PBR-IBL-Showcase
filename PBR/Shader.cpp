#include "Shader.h"
#include "FileSystem.h"

#include <iostream>
#include <vector>

bool _shCompileShader(GLuint iShader, const char *strName, const char *strCode)
{
	// compile shader
	glShaderSource(iShader, 1, &strCode, NULL);
	glCompileShader(iShader);

	// check for errors
	GLint iResult = GL_FALSE;
	int iInfoLogLength;
	glGetShaderiv(iShader, GL_COMPILE_STATUS, &iResult);
	glGetShaderiv(iShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
	if (iInfoLogLength > 0) {
		std::vector<char> acShaderErrorMessage(iInfoLogLength + 1);
		glGetShaderInfoLog(iShader, iInfoLogLength, NULL, &acShaderErrorMessage[0]);
		std::cout << "LOG: Error occurred while compiling " << strName << std::endl;
		std::cout << (char*)(&acShaderErrorMessage[0]) << std::endl;
	}

	return iResult == GL_TRUE ? true : false;
}

bool _shLinkShaders(GLuint &iProgram, GLuint iVertexShader, GLuint iFragmentShader)
{
	// link the program
	iProgram = glCreateProgram();
	glAttachShader(iProgram, iVertexShader);
	glAttachShader(iProgram, iFragmentShader);
	glLinkProgram(iProgram);

	// check for errors
	GLint iResult = GL_FALSE;
	int iInfoLogLength;
	glGetProgramiv(iProgram, GL_LINK_STATUS, &iResult);
	glGetProgramiv(iProgram, GL_INFO_LOG_LENGTH, &iInfoLogLength);
	if (iInfoLogLength > 0) {
		std::vector<char> acProgramErrorMessage(iInfoLogLength + 1);
		glGetProgramInfoLog(iProgram, iInfoLogLength, NULL, &acProgramErrorMessage[0]);
		std::cout << "LOG: Error occurred while linking shaders." << std::endl;
		std::cout << (char*)(&acProgramErrorMessage[0]) << std::endl;
	}

	glDetachShader(iProgram, iVertexShader);
	glDetachShader(iProgram, iFragmentShader);

	return iResult == GL_TRUE ? true : false;
}

CShader::CShader(const char *strVertexFilePath, const char *strFragmentFilePath)
{
	sh_iProgram = 0;

	// try reading vertex shader source code
	char *strVertexShaderCode = NULL;
	if (!_fsReadFile(&strVertexShaderCode, strVertexFilePath)) {
		free(strVertexShaderCode);
		return;
	}

	// try reading fragment shader source code
	char *strFragmentShaderCode = NULL;
	if (!_fsReadFile(&strFragmentShaderCode, strFragmentFilePath)) {
		free(strFragmentShaderCode);
		return;
	}

	// create shaders
	GLuint iVertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint iFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// try compiling vertex shader
	std::cout << "LOG: Loading shader " << strVertexFilePath << "/" << strFragmentFilePath << std::endl;
	bool bVertexCompiled = _shCompileShader(iVertexShader, strVertexFilePath, strVertexShaderCode);
	free(strVertexShaderCode);
	// try compiling fragment shader
	bool bFragmentCompiled = _shCompileShader(iFragmentShader, strFragmentFilePath, strFragmentShaderCode);
	free(strFragmentShaderCode);

	// if both shaders compiled
	if (bVertexCompiled && bFragmentCompiled) {
		// try linking them into a shader program
		if (!_shLinkShaders(sh_iProgram, iVertexShader, iFragmentShader)) {
			// clear program if error occured
			sh_iProgram = 0;
		}
	}

	glDeleteShader(iVertexShader);
	glDeleteShader(iFragmentShader);
}

CShader::~CShader(void)
{
	glDeleteProgram(sh_iProgram);
}

void CShader::Use(void)
{
	glUseProgram(sh_iProgram);
}

void CShader::SetBool(const std::string &strName, bool bValue)
{
	glUniform1i(glGetUniformLocation(sh_iProgram, strName.c_str()), (int)bValue);
}

void CShader::SetInt(const std::string &strName, int iValue)
{
	glUniform1i(glGetUniformLocation(sh_iProgram, strName.c_str()), iValue);
}

void CShader::SetFloat(const std::string &strName, float fValue)
{
	glUniform1f(glGetUniformLocation(sh_iProgram, strName.c_str()), fValue);
}

void CShader::SetVec2(const std::string &strName, glm::vec2 vValue)
{
	glUniform2fv(glGetUniformLocation(sh_iProgram, strName.c_str()), 1, &vValue[0]);
}

void CShader::SetVec2(const std::string &strName, float fValueX, float fValueY)
{
	glUniform2f(glGetUniformLocation(sh_iProgram, strName.c_str()), fValueX, fValueY);
}

void CShader::SetVec3(const std::string &strName, glm::vec3 vValue)
{
	glUniform3fv(glGetUniformLocation(sh_iProgram, strName.c_str()), 1, &vValue[0]);
}

void CShader::SetVec3(const std::string &strName, float fValueX, float fValueY, float fValueZ)
{
	glUniform3f(glGetUniformLocation(sh_iProgram, strName.c_str()), fValueX, fValueY, fValueZ);
}

void CShader::SetVec4(const std::string &strName, glm::vec4 vValue)
{
	glUniform4fv(glGetUniformLocation(sh_iProgram, strName.c_str()), 1, &vValue[0]);
}

void CShader::SetVec4(const std::string &strName, float fValueX, float fValueY, float fValueZ, float fValueW)
{
	glUniform4f(glGetUniformLocation(sh_iProgram, strName.c_str()), fValueX, fValueY, fValueZ, fValueW);
}

void CShader::SetMat2(const std::string &strName, glm::mat2 mValue)
{
	glUniformMatrix2fv(glGetUniformLocation(sh_iProgram, strName.c_str()), 1, GL_FALSE, &mValue[0][0]);
}

void CShader::SetMat3(const std::string &strName, glm::mat3 mValue)
{
	glUniformMatrix3fv(glGetUniformLocation(sh_iProgram, strName.c_str()), 1, GL_FALSE, &mValue[0][0]);
}

void CShader::SetMat4(const std::string &strName, glm::mat4 mValue)
{
	glUniformMatrix4fv(glGetUniformLocation(sh_iProgram, strName.c_str()), 1, GL_FALSE, &mValue[0][0]);
}
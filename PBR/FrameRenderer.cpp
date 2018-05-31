#include "FrameRenderer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "GeometryHelper.h"

CFrameRenderer::CFrameRenderer()
{
	fr_ulFlags = 0;
	fr_mProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	fr_amViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	fr_amViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	fr_amViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	fr_amViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	fr_amViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	fr_amViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

	glGenFramebuffers(1, &fr_uiFBO);
	glGenRenderbuffers(1, &fr_uiRBO);
}

CFrameRenderer::~CFrameRenderer()
{
	if (fr_pshShader != nullptr) {
		delete(fr_pshShader);
	}
}

void CFrameRenderer::PrepareTexture(unsigned int uiTexture)
{
	glGenTextures(1, &fr_uiTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, fr_uiTexture);
	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, fr_uiSize, fr_uiSize, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	if (fr_ulFlags & FF_LINEAR_TEXTURE_FILTER) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else if (fr_ulFlags & FF_MIPMAP_TEXTURE_FILTER) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (fr_ulFlags & FF_GENERATE_MIPMAP) {
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	glActiveTexture(GL_TEXTURE0);
	if (fr_ulFlags & FF_TEXTURE_2D) {
		glBindTexture(GL_TEXTURE_2D, uiTexture);
	}
	else if (fr_ulFlags & FF_TEXTURE_3D) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, uiTexture);
	}
}

void CFrameRenderer::SetCaptureSize(unsigned int uiSize)
{
	fr_uiSize = uiSize;
}

void CFrameRenderer::SetFlags(unsigned long ulNewFlags)
{
	fr_ulFlags = ulNewFlags;
}

void CFrameRenderer::SetShader(CShader *psh, const char *strMap)
{
	fr_pshShader = psh;
	fr_pshShader->Use();
	if (strMap[0] != 0) {
		fr_pshShader->SetInt(strMap, 0);
	}
	fr_pshShader->SetMat4("projection", fr_mProjection);
}

void CFrameRenderer::Capture(unsigned int uiSize, int iIndex)
{
	glBindRenderbuffer(GL_RENDERBUFFER, fr_uiRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, uiSize, uiSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fr_uiRBO);

	// don't forget to configure the viewport to the capture dimensions.
	glViewport(0, 0, uiSize, uiSize);
	glBindFramebuffer(GL_FRAMEBUFFER, fr_uiFBO);
	for (unsigned int i = 0; i < 6; ++i) {
		fr_pshShader->SetMat4("view", fr_amViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, fr_uiTexture, iIndex);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_ghRenderCube();
	}
}

unsigned int CFrameRenderer::CaptureCube(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fr_uiFBO);
	Capture(fr_uiSize, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return fr_uiTexture;
}

unsigned int CFrameRenderer::CaptureMipmapCube(int iMipmapLevels)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fr_uiFBO);
	for (unsigned int iMip = 0; iMip < iMipmapLevels; ++iMip) {
		unsigned int mipSize = fr_uiSize * std::pow(0.5, iMip);
		float roughness = (float)iMip / (float)(iMipmapLevels - 1);
		fr_pshShader->SetFloat("roughness", roughness);
		Capture(mipSize, iMip);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return fr_uiTexture;
}

unsigned int CFrameRenderer::GenerateBRDF(void)
{
	unsigned int brdfLUTTexture;
	glGenTextures(1, &brdfLUTTexture);

	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, fr_uiSize, fr_uiSize, 0, GL_RG, GL_FLOAT, 0);
	// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	glBindFramebuffer(GL_FRAMEBUFFER, fr_uiFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, fr_uiRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fr_uiSize, fr_uiSize);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, fr_uiSize, fr_uiSize);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_ghRenderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return brdfLUTTexture;
}

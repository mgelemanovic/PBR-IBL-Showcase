#include "Skybox.h"

#include <iostream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "FrameRenderer.h"
#include "GeometryHelper.h"
#include "GLContext.h"
#include "Shader.h"

CShader *CSkybox::_pshEquirectangular = nullptr;
CShader *CSkybox::_pshIrradiance = nullptr;
CShader *CSkybox::_pshPrefilter = nullptr;

CSkybox::CSkybox()
{
	sb_fExposure = 2.2f;
}

// use equirectangular HDR map as skybox
void CSkybox::Initialize(std::string strSkyboxName)
{
	std::cout << "LOG: Loading HDR skybox @" << strSkyboxName << std::endl;

	sb_strName = strSkyboxName.substr(strSkyboxName.find_last_of("\\") + 1, strSkyboxName.length());

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf(strSkyboxName.c_str(), &width, &height, &nrComponents, 0);
	unsigned int hdrTexture;
	if (data) {
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		// note how we specify the texture's data value to be float
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		std::cout << "ERR: Failed to load HDR skybox." << std::endl;
	}

	stbi_image_free(data);

	std::cout << "LOG: Mapping skybox." << std::endl;
	CFrameRenderer *pfr = CGLContext::_glGetContext()->GetFrameRenderer();
	pfr->SetCaptureSize(2048);
	pfr->SetFlags(FF_MIPMAP_TEXTURE_FILTER | FF_TEXTURE_2D);
	pfr->PrepareTexture(hdrTexture);
	pfr->SetShader(_pshEquirectangular, "equirectangularMap");
	sb_iEnvironmentMap = pfr->CaptureCube();

	glBindTexture(GL_TEXTURE_CUBE_MAP, sb_iEnvironmentMap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	std::cout << "LOG: Creating irradiance map." << std::endl;
	pfr->SetFlags(FF_LINEAR_TEXTURE_FILTER | FF_TEXTURE_3D);
	pfr->SetCaptureSize(32);
	pfr->SetShader(_pshIrradiance, "environmentMap");
	pfr->PrepareTexture(sb_iEnvironmentMap);
	sb_uiIrradianceMap = pfr->CaptureCube();

	std::cout << "LOG: Creating prefilter map." << std::endl;
	pfr->SetFlags(FF_MIPMAP_TEXTURE_FILTER | FF_GENERATE_MIPMAP | FF_TEXTURE_3D);
	pfr->SetCaptureSize(128);
	pfr->SetShader(_pshPrefilter, "environmentMap");
	pfr->PrepareTexture(sb_iEnvironmentMap);
	sb_uiPrefilterMap = pfr->CaptureMipmapCube(5);
}

void CSkybox::PreRender(void)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sb_uiIrradianceMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sb_uiPrefilterMap);
}

void CSkybox::Render(CShader *psh)
{
	// change depth function so depth test passes when values are equal to depth buffer's content
	glDepthFunc(GL_LEQUAL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sb_iEnvironmentMap);
	_ghRenderCube();
	// set depth function back to default
	glDepthFunc(GL_LESS);
}

void CSkybox::SetExposure(float fExposure)
{
	sb_fExposure = fExposure;
}

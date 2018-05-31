#ifndef PBR_SKYBOX_H
#define PBR_SKYBOX_H

#include <glad/glad.h>
#include <iostream>

class CShader;

class CSkybox {
public:
	GLuint sb_iEnvironmentMap;
	GLuint sb_uiIrradianceMap;
	GLuint sb_uiPrefilterMap;
	float sb_fExposure;
public:
	static CShader *_pshEquirectangular;
	static CShader *_pshIrradiance;
	static CShader *_pshPrefilter;
public:
	CSkybox(void);
	// use equirectangular HDR map as skybox
	void Initialize(std::string strSkyboxName);
	void PreRender(void);
	void Render(CShader *psh);
	void SetExposure(float fExposure);
};

#endif
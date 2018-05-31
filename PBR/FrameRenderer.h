#ifndef PBR_FRAMERENDERER_H
#define PBR_FRAMERENDERER_H

#include <glm/glm.hpp>

class CShader;

enum FrameFlags {
	FF_LINEAR_TEXTURE_FILTER = (1UL<<0),
	FF_MIPMAP_TEXTURE_FILTER = (1UL<<1),
	FF_TEXTURE_2D = (1UL<<2),
	FF_TEXTURE_3D = (1UL<<3),
	FF_GENERATE_MIPMAP = (1UL<<4),
};

class CFrameRenderer {
private:
	unsigned long fr_ulFlags;
	unsigned int fr_uiFBO;
	unsigned int fr_uiRBO;
	unsigned int fr_uiTexture;
	unsigned int fr_uiSize;
	glm::mat4 fr_mProjection;
	glm::mat4 fr_amViews[6];
public:
	CShader *fr_pshShader;
public:
	CFrameRenderer();
	~CFrameRenderer();

	void PrepareTexture(unsigned int uiTexture);
	void SetCaptureSize(unsigned int uiSize);
	void SetFlags(unsigned long ulNewFlags);
	void SetShader(CShader *psh, const char *strMap);
	void Capture(unsigned int uiSize, int iIndex);
	unsigned int CaptureCube(void);
	unsigned int CaptureMipmapCube(int iMipmapLevels);
	unsigned int GenerateBRDF(void);
};

#endif // PBR_FRAMERENDERER_H
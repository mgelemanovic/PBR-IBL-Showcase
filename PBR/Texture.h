#ifndef PBR_TEXTURE_H
#define PBR_TEXTURE_H

extern unsigned int _texLoadTextureFromFile(const char *strPath, const char *strDirectory);

class CTexture {
private:
	unsigned int tex_auiMaps[5];
public:
	CTexture();
	~CTexture();
	bool LoadTextures(const char *strPath, const char *strExtension);
	void Render(void);
};

#endif // PBR_TEXTURE_H
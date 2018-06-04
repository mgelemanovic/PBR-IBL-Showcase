#ifndef PBR_DATALOADER_H
#define PBR_DATALOADER_H

#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <string>

class CGLContext;

struct LightTemplate {
	glm::vec3 tp_vPosition;
	glm::vec3 tp_vColor;
};

struct ModelTemplate {
	std::string tp_strModelPath;
	std::string tp_strMaterialPath;
	std::string tp_strMaterialExtension;
	glm::vec3 tp_vPosition;
	glm::vec3 tp_vScale;
	glm::vec3 tp_vRotation;
	std::vector<int> tp_aiSkippedMeshes;
};

struct SkyboxTemplate {
	std::string tp_strPath;
	float tp_fExposure;
};

class CDataLoader {
private:
	std::vector<LightTemplate> dl_atpLights;
	std::vector<ModelTemplate> dl_atpModels;
	std::vector<SkyboxTemplate> dl_atpSkyboxes;
private:
	void Load_Light(std::ifstream &pf);
	void Load_Model(std::ifstream &pf);
	void Load_Skybox(std::ifstream &pf);
public:
	CDataLoader();
	void Load(std::string strFilePath);
	void Create(CGLContext *pgl);
};

#endif // PBR_DATALODER_H
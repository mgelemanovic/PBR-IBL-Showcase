#include "DataLoader.h"

#include "GLContext.h"
#include "Texture.h"
#include <iostream>

CTexture* _pbrCreatePBRTexture(const char *strFullPath, const char *strExtension)
{
	CTexture *ptex = new CTexture();
	if (!ptex->LoadTextures(strFullPath, strExtension)) {
		delete(ptex);
		ptex = nullptr;
	}
	return ptex;
}

glm::vec3 _dlGetVector(std::ifstream &pf)
{
	float afNumbers[] = { 0.0f, 0.0f, 0.0f };
	int iCounter = 0;
	std::string strWord;
	while (pf >> strWord) {
		afNumbers[iCounter++] = atof(strWord.c_str());
		if (iCounter == 3) {
			break;
		}
	}
	return glm::vec3(afNumbers[0], afNumbers[1], afNumbers[2]);
}

std::string _dlGetText(std::ifstream &pf)
{
	std::string strWord;
	while (pf >> strWord) {
		break;
	}
	return strWord;
}

float _dlGetNumber(std::ifstream &pf)
{
	float fResult = 0.0f;
	std::string strWord;
	while (pf >> strWord) {
		fResult = atof(strWord.c_str());
		break;
	}
	return fResult;
}

CDataLoader::CDataLoader()
{
}

void CDataLoader::Load_Light(std::ifstream &pf)
{
	LightTemplate tp;
	tp.tp_vColor = glm::vec3(1.0f);
	std::string strWord;
	while (pf >> strWord) {
		if (strWord.compare("_POSITION") == 0) {
			tp.tp_vPosition = _dlGetVector(pf);
		} else if (strWord.compare("_COLOR") == 0) {
			tp.tp_vColor = _dlGetVector(pf);
		} else if (strWord.compare("_END") == 0) {
			break;
		}
	}
	dl_atpLights.push_back(tp);
}

void CDataLoader::Load_Model(std::ifstream &pf)
{
	ModelTemplate tp;
	tp.tp_vScale = glm::vec3(1.0f);
	std::string strWord;
	while (pf >> strWord) {
		if (strWord.compare("_POSITION") == 0) {
			tp.tp_vPosition = _dlGetVector(pf);
		} else if (strWord.compare("_SCALE") == 0) {
			tp.tp_vScale = _dlGetVector(pf);
		} else if (strWord.compare("_ROTATION") == 0) {
			tp.tp_vRotation = _dlGetVector(pf);
		} else if (strWord.compare("_MATERIAL") == 0) {
			tp.tp_strMaterialPath = _dlGetText(pf);
		} else if (strWord.compare("_EXTENSION") == 0) {
			tp.tp_strMaterialExtension = _dlGetText(pf);
		} else if (strWord.compare("_PATH") == 0) {
			tp.tp_strModelPath = _dlGetText(pf);
		} else if (strWord.compare("_SKIP") == 0) {
			while (pf >> strWord) {
				int iSkippedMesh = atoi(strWord.c_str());
				if (iSkippedMesh == -1) {
					break;
				} else {
					tp.tp_aiSkippedMeshes.push_back(iSkippedMesh);
				}
			}
		} else if (strWord.compare("_END") == 0) {
			break;
		}
	}
	dl_atpModels.push_back(tp);
}

void CDataLoader::Load_Sphere(std::ifstream &pf)
{
	SphereTemplate tp;
	tp.tp_vScale = glm::vec3(1.0f);
	tp.tp_vColor = glm::vec3(1.0f);
	std::string strWord;
	while (pf >> strWord) {
		if (strWord.compare("_POSITION") == 0) {
			tp.tp_vPosition = _dlGetVector(pf);
		} else if (strWord.compare("_SCALE") == 0) {
			tp.tp_vScale = _dlGetVector(pf);
		} else if (strWord.compare("_ROTATION") == 0) {
			tp.tp_vRotation = _dlGetVector(pf);
		} else if (strWord.compare("_MATERIAL") == 0) {
			tp.tp_strMaterialPath = _dlGetText(pf);
		} else if (strWord.compare("_EXTENSION") == 0) {
			tp.tp_strMaterialExtension = _dlGetText(pf);
		} else if (strWord.compare("_COLOR") == 0) {
			tp.tp_vColor = _dlGetVector(pf);
		} else if (strWord.compare("_END") == 0) {
			break;
		}
	}
	dl_atpSpheres.push_back(tp);
}

void CDataLoader::Load_Skybox(std::ifstream &pf)
{
	SkyboxTemplate tp;
	tp.tp_fExposure = 2.2f;
	std::string strWord;
	while (pf >> strWord) {
		if (strWord.compare("_PATH") == 0) {
			tp.tp_strPath = _dlGetText(pf);
		} else if (strWord.compare("_EXPOSURE") == 0) {
			tp.tp_fExposure = _dlGetNumber(pf);
		} else if (strWord.compare("_END") == 0) {
			break;
		}
	}
	dl_atpSkyboxes.push_back(tp);
}

void CDataLoader::Load(std::string strFilePath)
{
	std::ifstream pfFile;
	pfFile.open(strFilePath);
	if (!pfFile.is_open()) {
		return;
	}

	std::string strWord;
	while (pfFile >> strWord) {
		if (strWord.compare("_LIGHT") == 0) {
			Load_Light(pfFile);
		} else if (strWord.compare("_MODEL") == 0) {
			Load_Model(pfFile);
		} else if (strWord.compare("_SPHERE") == 0) {
			Load_Sphere(pfFile);
		} else if (strWord.compare("_HDR") == 0) {
			Load_Skybox(pfFile);
		}
	}

	pfFile.close();
}

void CDataLoader::Create(CGLContext *pgl)
{
	unsigned int iSkyboxCount = dl_atpSkyboxes.size();
	std::cout << "LOG: Creating " << iSkyboxCount << " background(s)." << std::endl;
	for (unsigned int i = 0; i < iSkyboxCount; ++i) {
		SkyboxTemplate tp = dl_atpSkyboxes[i];
		CSkybox *psb = pgl->AddSkybox(tp.tp_strPath);
		psb->SetExposure(tp.tp_fExposure);
	}

	unsigned int iLightCount = dl_atpLights.size();
	std::cout << "LOG: Creating " << iLightCount << " light source(s)." << std::endl;
	for (unsigned int i = 0; i < iLightCount; ++i) {
		LightTemplate tp = dl_atpLights[i];
		pgl->AddLight(tp.tp_vPosition, tp.tp_vColor);
	}

	unsigned int iModelCount = dl_atpModels.size();
	std::cout << "LOG: Creating " << iModelCount << " model(s)." << std::endl;
	for (unsigned int i = 0; i < iModelCount; ++i) {
		ModelTemplate tp = dl_atpModels[i];
		CModelRenderable *pre = pgl->AddModel(tp.tp_strModelPath.c_str());
		CTexture *ptex = _pbrCreatePBRTexture(tp.tp_strMaterialPath.c_str(), tp.tp_strMaterialExtension.c_str());
		pre->SetPBRTexture(ptex);
		pre->SetPosition(tp.tp_vPosition);
		pre->SetScale(tp.tp_vScale);
		pre->SetRotation(tp.tp_vRotation);
		for (unsigned int j = 0; j < tp.tp_aiSkippedMeshes.size(); ++j) {
			pre->SkipMesh(tp.tp_aiSkippedMeshes[j]);
		}
	}

	unsigned int iSphereCount = dl_atpSpheres.size();
	std::cout << "LOG: Creating " << iSphereCount << " sphere(s)." << std::endl;
	for (unsigned int i = 0; i < iSphereCount; ++i) {
		SphereTemplate tp = dl_atpSpheres[i];
		CRenderable *pre = pgl->AddSphere();
		CTexture *ptex = _pbrCreatePBRTexture(tp.tp_strMaterialPath.c_str(), tp.tp_strMaterialExtension.c_str());
		pre->SetTexture(ptex);
		pre->SetPosition(tp.tp_vPosition);
		pre->SetScale(tp.tp_vScale);
		pre->SetRotation(tp.tp_vRotation);
		pre->SetColor(tp.tp_vColor);
	}
}

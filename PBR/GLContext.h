#ifndef PBR_OGLCONTEXT_H
#define PBR_OGLCONTEXT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "FrameRenderer.h"
#include "Camera.h"
#include "Model.h"
#include "Skybox.h"
#include "Light.h"
#include "UserInterface.h"

class CShader;

enum GLContextFlags {
	GLF_UPDATE = (1UL<<0),
	GLF_PROCESS_KEYBOARD = (1UL<<1),
	GLF_PROCESS_MOUSE = (1UL<<2),
	GLF_FIRST_MOUSE_MOVE = (1UL<<3),
	GLF_RENDER_LIGHTS = (1UL<<4),
};

struct RenderingOption {
	const char *ro_strName;
	bool ro_bNextLine;
};

class CGLContext {
private:
	GLFWwindow *gl_iWindow;
	GLuint gl_iWidth, gl_iHeight;

	unsigned long gl_glfContextFlags;

	GLboolean gl_bPausedSimulation;
	GLfloat gl_fDeltaTime;
	GLfloat gl_fLastFrame;

	glm::vec2 gl_vLastMousePosition;
	GLboolean gl_abKeyDown[1024];
	GLboolean gl_abKeyUsed[1024];

	CUserInterface *gl_puiInterface;
	CFrameRenderer *gl_pfrRenderer;
	CShader *gl_pshPBRShader;
	CShader *gl_pshSkyboxShader;
	CCamera *gl_pcActiveCamera;
	std::vector<CLight*> gl_alLights;

	GLuint gl_uiBRDF;

	CGLContext();
public:
	int gl_iActiveSkybox;
	std::vector<CSkybox*> gl_apsbSkyboxes;
	int gl_iActiveModel;
	std::vector<CModelRenderable*> gl_apreLoadedModels;
	int gl_iRenderingMode;
	std::vector<RenderingOption> gl_aroOptions;
	bool gl_bLightsEnabled;
	bool gl_bBackgroundEnabled;
	float gl_fGamma;
public:
	~CGLContext();
	void Initialize(int argc, char **argv, const char *strWindowName, unsigned long glfFlags);
	void CreatePBRShader(void);
	void CreateSkyboxShader(void);
	void AddRenderingOption(const char *strName, bool bNextLine);
	void AddLight(glm::vec3 vPosition, glm::vec3 vColor);
	CModelRenderable *AddModel(const char *strModelPath);
	CSkybox *AddSkybox(std::string strName);
	void CreateBRDF(void);
	void Start(void);
	void UpdateTime(void);

	void ProcessInput(void);
	void RenderScene(void);
	void Update(void);

	void HandleInput(GLFWwindow* window, int key, int scancode, int action, int mode);
	void HandleMouse(GLFWwindow* window, double xpos, double ypos);
	void HandleScroll(GLFWwindow* window, double xoffset, double yoffset);

	void ResetViewport(void);

	bool IsKeyDown(int iKey);
	bool IsKeyPressed(int iKey);

	CCamera *GetActiveCamera(void);
	void ResetCamera(void);
	CFrameRenderer *GetFrameRenderer(void);
private:
	static CGLContext *_glContext;
public:
	static CGLContext *_glGetContext(void);
};

#endif // PBR_OGLCONTEXT_H
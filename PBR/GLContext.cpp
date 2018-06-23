#include "GLContext.h"

#include <iostream>
#include "Shader.h"
#include "GeometryHelper.h"

#define GL_WINDOW_WIDTH 1600
#define GL_WINDOW_HEIGHT 900

void _glHandleReshape(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void _glHandleInput(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	CGLContext::_glGetContext()->HandleInput(window, key, scancode, action, mode);
}

void _glHandleMouse(GLFWwindow* window, double xpos, double ypos)
{
	CGLContext::_glGetContext()->HandleMouse(window, xpos, ypos);
}

void _glHandleScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	CGLContext::_glGetContext()->HandleScroll(window, xoffset, yoffset);
}

CGLContext *CGLContext::_glContext = 0;

CGLContext *CGLContext::_glGetContext(void)
{
	if (_glContext == NULL) {
		_glContext = new CGLContext();
	}
	return _glContext;
}

CGLContext::CGLContext() : 
	gl_iWidth(GL_WINDOW_WIDTH), gl_iHeight(GL_WINDOW_HEIGHT), gl_glfContextFlags(0),
	gl_bPausedSimulation(GL_FALSE), gl_fDeltaTime(0.0f), gl_fLastFrame(0.0f)
{
	gl_vLastMousePosition = glm::vec2(GL_WINDOW_WIDTH / 2.0f, GL_WINDOW_HEIGHT / 2.0f);
	gl_pshPBRShader = NULL;
	gl_pshSkyboxShader = NULL;
	gl_pcActiveCamera = NULL;
	gl_iActiveSkybox = 0;
	gl_iActiveModel = 0;
	gl_iRenderingMode = 0;
	gl_bLightsEnabled = true;
	gl_bBackgroundEnabled = true;
	gl_fGamma = 2.2f;

	ResetCamera();
	AddRenderingOption("Default", false);
}

CGLContext::~CGLContext()
{
}

void CGLContext::Initialize(int argc, char **argv, const char *strWindowName, unsigned long glfFlags)
{
	std::cout << "LOG: Initializing OpenGL context." << std::endl;

	// remember desired flags
	gl_glfContextFlags = glfFlags;
	gl_glfContextFlags |= GLF_FIRST_MOUSE_MOVE;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	gl_iWindow = glfwCreateWindow(GL_WINDOW_WIDTH, GL_WINDOW_HEIGHT, strWindowName, NULL, NULL);
	glfwMakeContextCurrent(gl_iWindow);
	if (gl_iWindow == NULL) {
		std::cout << "ERR:	Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}

	gl_puiInterface = new CUserInterface(gl_iWindow);
	glfwSetFramebufferSizeCallback(gl_iWindow, _glHandleReshape);
	glfwSetKeyCallback(gl_iWindow, _glHandleInput);
	glfwSetCursorPosCallback(gl_iWindow, _glHandleMouse);
	glfwSetScrollCallback(gl_iWindow, _glHandleScroll);

	// tell GLFW to capture our mouse
	glfwSetInputMode(gl_iWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "ERR: Failed to initialize GLAD." << std::endl;
		return;
	}

	CSkybox::_pshEquirectangular = new CShader("Cubemap.vs", "EquirectangularToCubemap.fs");
	CSkybox::_pshIrradiance = new CShader("Cubemap.vs", "IrradianceConvolution.fs");
	CSkybox::_pshPrefilter = new CShader("Cubemap.vs", "Prefilter.fs");
	gl_pfrRenderer = new CFrameRenderer();

	// additional rendering setup
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	CreatePBRShader();
	CreateSkyboxShader();
	CreateBRDF();
}


void CGLContext::CreatePBRShader(void)
{
	CShader *psh = new CShader("PBR.vs", "PBR.fs");
	if (psh == NULL || psh->sh_iProgram == 0) {
		return;
	}
	gl_pshPBRShader = psh;

	gl_pshPBRShader->Use();
	gl_pshPBRShader->SetInt("irradianceMap", 0);
	gl_pshPBRShader->SetInt("prefilterMap", 1);
	gl_pshPBRShader->SetInt("brdfLUT", 2);

	gl_pshPBRShader->SetInt("albedoMap", 3);
	gl_pshPBRShader->SetInt("normalMap", 4);
	gl_pshPBRShader->SetInt("metallicMap", 5);
	gl_pshPBRShader->SetInt("roughnessMap", 6);
	gl_pshPBRShader->SetInt("aoMap", 7);
}

void CGLContext::CreateSkyboxShader(void)
{
	CShader *psh = new CShader("Skybox.vs", "Skybox.fs");
	if (psh == NULL || psh->sh_iProgram == 0) {
		return;
	}
	gl_pshSkyboxShader = psh;

	gl_pshSkyboxShader->Use();
	gl_pshSkyboxShader->SetInt("skybox", 0);
}

void CGLContext::AddRenderingOption(const char *strName, bool bNextLine)
{
	RenderingOption ro;
	ro.ro_strName = strName;
	ro.ro_bNextLine = bNextLine;
	gl_aroOptions.push_back(ro);
}

void CGLContext::AddLight(glm::vec3 vPosition, glm::vec3 vColor)
{
	CLight *pl = new CLight(vPosition, vColor);
	gl_alLights.push_back(pl);
}

CModelRenderable *CGLContext::AddModel(const char *strModelPath)
{
	CModelRenderable *pre = new CModelRenderable(std::string(strModelPath));
	if (pre == NULL) {
		return NULL;
	}

	gl_apreLoadedModels.push_back(pre);

	return pre;
}

CSkybox *CGLContext::AddSkybox(std::string strName)
{
	CSkybox *psb = new CSkybox();
	psb->Initialize(strName);
	gl_apsbSkyboxes.push_back(psb);
	return psb;
}

void CGLContext::CreateBRDF(void)
{
	std::cout << "LOG: Creating BRDF." << std::endl;

	CShader *pshBRDF = new CShader("BRDF.vs", "BRDF.fs");
	gl_pfrRenderer->SetFlags(0);
	gl_pfrRenderer->SetCaptureSize(512);
	gl_pfrRenderer->SetShader(pshBRDF, "");
	gl_uiBRDF = gl_pfrRenderer->GenerateBRDF();
	delete(pshBRDF);
}

void CGLContext::Start(void)
{
	std::cout << "LOG: Starting simulation." << std::endl;

	// ensure proper viewport dimensions
	ResetViewport();

	while (!glfwWindowShouldClose(gl_iWindow)) {
		UpdateTime();

		ProcessInput();
		Update();
		gl_puiInterface->PreRender();
		RenderScene();
		gl_puiInterface->Render();

		glfwSwapBuffers(gl_iWindow);
		glfwPollEvents();
	}

	gl_puiInterface->Shutdown();
	glfwTerminate();
}

void CGLContext::UpdateTime(void)
{
	float fCurrentFrame = glfwGetTime();
	gl_fDeltaTime = fCurrentFrame - gl_fLastFrame;
	gl_fLastFrame = fCurrentFrame;
}

void CGLContext::ProcessInput(void)
{
	if (IsKeyPressed(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(gl_iWindow, true);
	}

	if (IsKeyPressed(GLFW_KEY_P)) {
		gl_bPausedSimulation = !gl_bPausedSimulation;
		if (gl_bPausedSimulation) {
			glfwSetInputMode(gl_iWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			glfwSetInputMode(gl_iWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			gl_glfContextFlags |= GLF_FIRST_MOUSE_MOVE;
		}
	}
	
	if (IsKeyPressed(GLFW_KEY_B)) {
		std::cout << "LOG: Changing background." << std::endl;
		gl_iActiveSkybox = (gl_iActiveSkybox + 1) % gl_apsbSkyboxes.size();
	}
	if (IsKeyPressed(GLFW_KEY_N)) {
		std::cout << "LOG: Changing model." << std::endl;
		gl_iActiveModel = (gl_iActiveModel + 1) % gl_apreLoadedModels.size();
	}

	// don't allow camera movement while paused
	if (gl_bPausedSimulation) {
		return;
	}

	if (IsKeyDown(GLFW_KEY_W)) {
		gl_pcActiveCamera->ProcessKeyboard(CM_FORWARD, gl_fDeltaTime);
	}
	if (IsKeyDown(GLFW_KEY_S)) {
		gl_pcActiveCamera->ProcessKeyboard(CM_BACKWARD, gl_fDeltaTime);
	}
	if (IsKeyDown(GLFW_KEY_A)) {
		gl_pcActiveCamera->ProcessKeyboard(CM_LEFT, gl_fDeltaTime);
	}
	if (IsKeyDown(GLFW_KEY_D)) {
		gl_pcActiveCamera->ProcessKeyboard(CM_RIGHT, gl_fDeltaTime);
	}
	if (IsKeyDown(GLFW_KEY_SPACE)) {
		gl_pcActiveCamera->ProcessKeyboard(CM_UP, gl_fDeltaTime);
	}
	if (IsKeyDown(GLFW_KEY_C)) {
		gl_pcActiveCamera->ProcessKeyboard(CM_DOWN, gl_fDeltaTime);
	}
}

void CGLContext::RenderScene(void)
{
	// clear screen
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup projection and view matrices
	glm::mat4 projection = glm::perspective(glm::radians(gl_pcActiveCamera->c_fZoom), (float)gl_iWidth / (float)gl_iHeight, 0.1f, 100.0f);
	glm::mat4 view = gl_pcActiveCamera->GetViewMatrix();

	// activate PBR shader
	gl_pshPBRShader->Use();
	gl_pshPBRShader->SetMat4("projection", projection);
	gl_pshPBRShader->SetMat4("view", view);
	gl_pshPBRShader->SetVec3("camPos", gl_pcActiveCamera->c_vPosition);
	gl_pshPBRShader->SetInt("renderingMode", gl_iRenderingMode);

	// setup textures
	CSkybox *psb = gl_apsbSkyboxes[gl_iActiveSkybox];
	psb->PreRender();
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gl_uiBRDF);

	// setup lights
	GLuint iLightCount = gl_bLightsEnabled ? gl_alLights.size() : 0;
	gl_pshPBRShader->SetInt("lightCount", iLightCount);
	for (GLuint i = 0; i < iLightCount; ++i) {
		gl_alLights[i]->Prepare(*gl_pshPBRShader, i);
		if (gl_glfContextFlags & GLF_RENDER_LIGHTS) {
			gl_alLights[i]->Render(*gl_pshPBRShader);
		}
	}

	// render the loaded models
	if (gl_apreLoadedModels.size() > 0) {
		gl_pshPBRShader->SetFloat("gamma", gl_fGamma);
		gl_apreLoadedModels[gl_iActiveModel]->Render(gl_pshPBRShader);
	}

	if (gl_bBackgroundEnabled) {
		// render skybox at the end
		gl_pshSkyboxShader->Use();
		// remove translation from the view matrix
		view = glm::mat4(glm::mat3(gl_pcActiveCamera->GetViewMatrix()));
		gl_pshSkyboxShader->SetMat4("view", view);
		gl_pshSkyboxShader->SetMat4("projection", projection);
		gl_pshSkyboxShader->SetFloat("gamma", gl_fGamma);
		psb->Render(gl_pshSkyboxShader);
	}
}

void CGLContext::HandleInput(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key < 0 && key >= 1024) {
		return;
	}

	if (action == GLFW_PRESS) {
		gl_abKeyDown[key] = GL_TRUE;
	} else if (action == GLFW_RELEASE) {
		gl_abKeyDown[key] = GL_FALSE;
		gl_abKeyUsed[key] = GL_FALSE;
	}
}

void CGLContext::HandleMouse(GLFWwindow *window, double xpos, double ypos)
{
	if (gl_bPausedSimulation) {
		return;
	}

	if (gl_glfContextFlags & GLF_FIRST_MOUSE_MOVE) {
		gl_vLastMousePosition = glm::vec2(xpos, ypos);
		gl_glfContextFlags &= !GLF_FIRST_MOUSE_MOVE;
	}

	float xoffset = xpos - gl_vLastMousePosition.x;
	// reversed since y-coordinates go from bottom to top
	float yoffset = gl_vLastMousePosition.y - ypos;

	gl_vLastMousePosition = glm::vec2(xpos, ypos);

	gl_pcActiveCamera->ProcessMouseMovement(xoffset, yoffset);
}

void CGLContext::HandleScroll(GLFWwindow *window, double xoffset, double yoffset)
{
	if (gl_bPausedSimulation) {
		return;
	}

	float fSpeed = gl_pcActiveCamera->c_fMovementSpeed;
	float fMinSpeed = 10.0f;
	float fMaxSpeed = 50.0f;
	if (fSpeed >= fMinSpeed && fSpeed <= fMaxSpeed) {
		fSpeed += yoffset;
	}
	if (fSpeed <= fMinSpeed) {
		fSpeed = fMinSpeed;
	}
	if (fSpeed >= fMaxSpeed) {
		fSpeed = fMaxSpeed;
	}
	gl_pcActiveCamera->c_fMovementSpeed = fSpeed;
}

void CGLContext::ResetViewport(void)
{
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(gl_iWindow, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);
}

bool CGLContext::IsKeyDown(int iKey)
{
	return gl_abKeyDown[iKey];
}

bool CGLContext::IsKeyPressed(int iKey)
{
	bool bIsPressed = gl_abKeyDown[iKey] && !gl_abKeyUsed[iKey];
	if (bIsPressed) {
		gl_abKeyUsed[iKey] = GL_TRUE;
	}
	return bIsPressed;
}

CCamera *CGLContext::GetActiveCamera(void)
{
	return gl_pcActiveCamera;
}

void CGLContext::ResetCamera(void)
{
	if (gl_pcActiveCamera != NULL) {
		delete(gl_pcActiveCamera);
		gl_pcActiveCamera = NULL;
	}
	gl_pcActiveCamera = new CCamera(glm::vec3(0.0, 0.0, 20.0));
}

CFrameRenderer *CGLContext::GetFrameRenderer(void)
{
	return gl_pfrRenderer;
}

void CGLContext::Update(void)
{
	if (gl_bPausedSimulation) {
		return;
	}
}
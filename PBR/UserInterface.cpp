#include "UserInterface.h"

#include <glad/glad.h>
#include "GLContext.h"

CUserInterface::CUserInterface(GLFWwindow *iWindow)
{
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(iWindow, true);
	ImGui::StyleColorsDark();
}


CUserInterface::~CUserInterface()
{
}

void CUserInterface::PreRender(void)
{
	ImGui_ImplGlfwGL3_NewFrame();
	CGLContext *pgl = CGLContext::_glGetContext();

	if (ImGui::CollapsingHeader("Rendering data", 0, true, true)) {
		int iModel = pgl->gl_iActiveModel;
		int iModelCount = pgl->gl_apreLoadedModels.size();
		CModelRenderable *pre = pgl->gl_apreLoadedModels[iModel];
		if (pre->re_strName.empty()) {
			ImGui::Text("Current model: Sphere %i", iModel);
		} else {
			ImGui::Text("Current model: %s", pre->re_strName.c_str());
		}
		ImGui::SliderFloat("Model exposure", &pgl->gl_fModelExposure, 1.0f, 4.0f);
		if (ImGui::Button("Prev Model")) {
			if (iModel == 0) {
				iModel = iModelCount;
			}
			pgl->gl_iActiveModel = iModel - 1;
			pgl->gl_fModelExposure = 2.2f;
		}
		ImGui::SameLine();
		if (ImGui::Button("Next Model")) {
			pgl->gl_iActiveModel = (iModel + 1) % iModelCount;
			pgl->gl_fModelExposure = 2.2f;
		}

		int iSkybox = pgl->gl_iActiveSkybox;
		int iSkyboxCount = pgl->gl_apsbSkyboxes.size();
		CSkybox *psb = pgl->gl_apsbSkyboxes[iSkybox];
		ImGui::Text("Current skybox: %s", psb->sb_strName.c_str());
		ImGui::SliderFloat("Skybox exposure", &pgl->gl_fSkyboxExposure, 1.0f, 4.0f);
		if (ImGui::Button("Prev Skybox")) {
			if (iSkybox == 0) {
				iSkybox = iSkyboxCount;
			}
			pgl->gl_iActiveSkybox = iSkybox - 1;
			pgl->gl_fSkyboxExposure = pgl->gl_apsbSkyboxes[pgl->gl_iActiveSkybox]->sb_fExposure;
		}
		ImGui::SameLine();
		if (ImGui::Button("Next Skybox")) {
			pgl->gl_iActiveSkybox = (iSkybox + 1) % iSkyboxCount;
			pgl->gl_fSkyboxExposure = pgl->gl_apsbSkyboxes[pgl->gl_iActiveSkybox]->sb_fExposure;
		}
	}

	if (ImGui::CollapsingHeader("Rendering options", 0, true, true)) {
		int iActiveRenderingMode = pgl->gl_iRenderingMode;
		int iModeCount = pgl->gl_aroOptions.size();
		for (int i = 0; i < iModeCount; ++i) {
			RenderingOption &ro = pgl->gl_aroOptions[i];
			if (ImGui::RadioButton(ro.ro_strName, iActiveRenderingMode == i)) {
				pgl->gl_iRenderingMode = i;
			}
			if (i != (iModeCount - 1) && !ro.ro_bNextLine) {
				ImGui::SameLine();
			}
		}
		if (ImGui::Button("Lights")) {
			pgl->gl_bLightsEnabled = !pgl->gl_bLightsEnabled;
		}
		ImGui::SameLine();
		if (ImGui::Button("Background")) {
			pgl->gl_bBackgroundEnabled = !pgl->gl_bBackgroundEnabled;
		}
	}

	if (ImGui::CollapsingHeader("Camera", 0, true, true)) {

		CCamera *pc = pgl->GetActiveCamera();
		ImGui::Text("Speed: %f", pc->c_fMovementSpeed);
		ImGui::Text("FOV:"); ImGui::SameLine();
		ImGui::SliderFloat("", &pc->c_fZoom, 45.0f, 90.0f);
		if (ImGui::Button("Reset Camera")) {
			pgl->ResetCamera();
		}
	}

	if (ImGui::CollapsingHeader("Application Info", 0, true, true)) {
		ImGui::Text("OpenGL Version : %s", (char*)glGetString(GL_VERSION));
		ImGui::Text("Hardware Informations : %s", (char*)glGetString(GL_RENDERER));
		ImGui::Text("\nFramerate %.2f FPS / Frametime %.4f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
	}

	if (ImGui::CollapsingHeader("About", 0, true, true)) {
		ImGui::Text("PBR and IBL Demo by Mladen Gelemanovic\n\nEmail: mladen.gelemanovic@fer.hr");
	}
}

void CUserInterface::Render(void)
{
	ImGui::Render();
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}

void CUserInterface::Shutdown(void)
{
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
}

#ifndef PBR_USERINTERFACE_H
#define PBR_USERINTERFACE_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

class CUserInterface {
public:
	CUserInterface(GLFWwindow *iWindow);
	~CUserInterface();
	void PreRender(void);
	void Render(void);
	void Shutdown(void);
};

#endif // PBR_USERINTERFACE_H
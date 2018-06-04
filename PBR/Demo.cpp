#include "GLContext.h"
#include "DataLoader.h"

int main(int argc, char **argv)
{

	CGLContext *pgl = CGLContext::_glGetContext();
	pgl->Initialize(argc, argv, "PBR Demo", 
		GLF_PROCESS_KEYBOARD | GLF_PROCESS_MOUSE | GLF_UPDATE);

	CDataLoader data;
	data.Load("..\\Resources\\params.txt");
	data.Create(pgl);

	pgl->AddRenderingOption("Albedo", false);
	pgl->AddRenderingOption("Normal", true);
	pgl->AddRenderingOption("Metal", false);
	pgl->AddRenderingOption("Roughness", false);
	pgl->AddRenderingOption("AO", true);
	pgl->AddRenderingOption("Light", false);
	pgl->AddRenderingOption("Fresnel", false);
	pgl->AddRenderingOption("Irradiance", true);
	pgl->AddRenderingOption("Diffuse", false);
	pgl->AddRenderingOption("Prefilter", false);
	pgl->AddRenderingOption("BRDF", true);
	pgl->AddRenderingOption("Specular", false);
	pgl->AddRenderingOption("Ambient", false);

	pgl->Start();

	return 0;
}
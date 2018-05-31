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

	pgl->Start();

	return 0;
}
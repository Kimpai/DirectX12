#include <crtdbg.h>
#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	SystemClass* system;
	bool result;

	//Create the system object
	system = new SystemClass;
	if (!system)
		return 0;

	//Initialize and run the system object
	result = system->Initialize();
	if (result)
		system->Run();

	//Shutdown and release the system object
	system->Shutdown();
	delete system;
	system = nullptr;

	return 0;
}
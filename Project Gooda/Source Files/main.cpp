#include <crtdbg.h>
#include "Gooda.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	GoodaDevice* device;

	//Create the device object
	device = new GoodaDevice;
	if (!device)
		return 0;

	//Initialize and run the device object
	device->Initialize();
	device->Run();

	//Shutdown and release the device object
	device->Shutdown();
	delete device;
	device = nullptr;

	return 0;
}
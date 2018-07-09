#include <crtdbg.h>
#include "GD.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	GoodaDevice* device;

	//Create the device object
	device = new GoodaDevice();
	assert(device);

	//Initialize and run the device object
	device->Run();

	//Release the Gooda object
	device->Release();

	return 0;
}
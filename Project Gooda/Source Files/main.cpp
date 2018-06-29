#include <crtdbg.h>
#include "Gooda.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	std::unique_ptr<GoodaDevice> device;

	//Create the device object
	device = std::make_unique<GoodaDevice>();
	assert(device);

	//Initialize and run the device object
	device->Run();

	return 0;
}
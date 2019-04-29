#include <crtdbg.h>
#include "Gooda/GoodaDriver.h"

using namespace GoodaCore;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pScmdline, int iCmdshow)
{
	GoodaDriver::Instance()->Init();
	GoodaDriver::Instance()->Run();
	GoodaDriver::Instance()->Destroy();

	return 0;
}
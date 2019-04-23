#include <crtdbg.h>
#include <Gooda.h>
#include "Gooda/GoodaDriver.h"

using namespace GoodaCore;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pScmdline, int iCmdshow)
{
	MSG msg;
	bool done, result;

	//Initialize the message structure
	ZeroMemory(&msg, sizeof(MSG));

	GoodaDriver::Instance()->Init();

	//Loop until there is a quit message from the window or the user
	done = false;
	while (!done)
	{
		//Handle the windows messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//If the windows signals to end the application then exit out
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//Otherwise do the frame processing
			result = GoodaDriver::Instance()->Frame();
			if (!result)
			{
				done = true;
			}
		}
	}

	GoodaDriver::Instance()->Destroy();

	return 0;
}
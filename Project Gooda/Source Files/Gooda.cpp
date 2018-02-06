#include "Gooda.h"

GoodaDevice::GoodaDevice()
{
	m_Input = nullptr;
	m_Driver = nullptr;
	m_Console = nullptr;
	m_Lua = nullptr;
	m_Camera = nullptr;
}

GoodaDevice::GoodaDevice(const GoodaDevice& other)
{

}

GoodaDevice::~GoodaDevice()
{

}

void GoodaDevice::Initialize()
{
	int screenHeight, screenWidth;

	//Initialize the width and height of the screen to zero
	screenHeight = 0;
	screenWidth = 0;

	//Initialize the windows api
	InitializeWindows(screenHeight, screenWidth);

	//Create the input object
	m_Input = new Input();
	assert(m_Input);

	//Initialize the input object
	m_Input->Initialize(m_hwnd);

	//Create the camera object
	m_Camera = new Camera(m_Input);
	assert(m_Camera);

	//Create the graphics object
	m_Driver = new GoodaDriver();
	assert(m_Driver);

	//Initialize the graphics object
	m_Driver->Initialize(m_hwnd, m_Camera);

	//Create console object
	m_Console = new Console();
	assert(m_Console);

	//Initialize console object
	m_Console->Initialize();


	//Create lua object
	m_Lua = new Lua();
	assert(m_Lua);

	//Initialize lua object
	m_Lua->Initialize();

	m_Lua->PushCFunction(m_Lua->Print);
	m_Lua->SetGlobal("Print");
	m_Lua->PushCFunction(m_Driver->RenderQuad);
	m_Lua->SetGlobal("RenderQuad");
}

void GoodaDevice::Shutdown()
{
	//Release the input object
	if (m_Input)
	{
		delete m_Input;
		m_Input = nullptr;
	}

	//Release the graphics object
	if (m_Driver)
	{
		delete m_Driver;
		m_Driver = nullptr;
	}

	//Release the camera object
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	//Release console object
	if (m_Console)
	{
		delete m_Console;
		m_Console = nullptr;
	}

	//Release lua object
	if (m_Lua)
	{
		delete m_Lua;
		m_Lua = nullptr;
	}

	//Shutdown the window
	ShutdownWindows();

	return;
}

void GoodaDevice::Run()
{
	MSG msg;
	bool done, result;

	//Initialize the message structure
	ZeroMemory(&msg, sizeof(MSG));

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
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}

	return;
}

LRESULT GoodaDevice::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		//Check if a key has been pressed on the keyboard
		case WM_KEYDOWN:
		{
			//If a key is pressed send it to input object so it can record that state
			m_Input->ProcessKeyboardMessage(umsg, wparam, lparam);
			break;
		}

		//Check if a key has been released on the keyboard
		case WM_KEYUP:
		{
			//If a key has been released then send it to the input object so it can unset the state for that key
			m_Input->ProcessKeyboardMessage(umsg, wparam, lparam);
			break;
		}

		case WM_MOUSEMOVE:
		{
			//If mouse was moved send to input object to update the camera
			m_Input->ProcessMouseMessage(umsg, wparam, lparam);
			break;
		}
	
		//Any other message send to the default message handler as out application won't use them
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}

	return 0;
}

bool GoodaDevice::Frame()
{
	std::string input;

	//Do the console frame processing
	if (m_Console->Frame(input))
	{
		if (!m_Lua->DoString(input.c_str()))
			std::cout << "Lua Error" << std::endl;
	}

	//Do the frame processing for input and camera
	if (m_hwnd == GetFocus())
	{
		//Update the mouse and keyboard
		m_Input->Frame();

		//Check if user wan't to exit application
		if (m_Input->IsKeyPressed(VK_ESCAPE))
			return false;

		m_Camera->Frame();
	}
	
	//Do the frame processing for the graphics object
	m_Driver->Frame(m_Camera);

	return true;
}

void GoodaDevice::InitializeWindows(int& screenHeight, int& screenWidth)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//Get an external pointer to this object
	ApplicationHandle = this;

	//Get the instance of this application
	m_hinstance = GetModuleHandle(NULL);

	//Give the application a name
	m_applicationName = L"Gooda Engine";

	//Setup the windows class with the default settings
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = (LPCSTR)m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//Register the window class
	RegisterClassEx(&wc);

	//Determine the resolution of the clients desktop screen
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	screenWidth = GetSystemMetrics(SM_CXSCREEN);

	//Setup the screen settings depending on whether it is running in full screen or in window mode
	if (FULL_SCREEN)
	{
		//If full screen set the screen maximum size of the users desktop and 32bit
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Change the display settings to full screen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//Set the position of window to the top left corner
		posX = posY = 0;
	}
	else
	{
		//If windowed then set it 800x600 resolution
		screenWidth = SCREEN_WIDTH;
		screenHeight = SCREEN_HEIGHT;

		//Place the window in the middle of the screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//Create the window with the screen settings and get the handle to it
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, (LPCSTR)m_applicationName, (LPCSTR)m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth,
		screenHeight, NULL, NULL, m_hinstance, NULL);

	//Bring the window up on the screen and set it as main focus
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//Hide the mouse cursor
	ShowCursor(false);

	return;
}

void GoodaDevice::ShutdownWindows()
{
	//Show the mouse cursor
	ShowCursor(true);

	//Fix the display settings if leaving full screen
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//Remove the window
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//Remove the application instance
	UnregisterClass((LPCSTR)m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//Release the pointer to this class
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		//Check if the window is being destroyed
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}

		//Check if the window is being closed
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}

		case WM_ACTIVATEAPP:
		case WM_INPUT:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
		{
			Mouse::ProcessMessage(umessage, wparam, lparam);
			break;
		}

		//All other messages pass to the message handler in the system class
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}

	return 0;
}
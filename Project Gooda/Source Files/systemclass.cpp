#include "systemclass.h"

SystemClass::SystemClass()
{
	m_Input = nullptr;
	m_Graphics = nullptr;
}

SystemClass::SystemClass(const SystemClass& other)
{

}

SystemClass::~SystemClass()
{

}

bool SystemClass::Initialize()
{
	int screenHeight, screenWidth;
	bool result;

	//Initialize the width and height of the screen to zero
	screenHeight = 0;
	screenWidth = 0;

	//Initialize the windows api
	InitializeWindows(screenHeight, screenWidth);

	//Create the input object
	m_Input = new InputClass();
	if (!m_Input)
		return false;

	//Initialize the input object
	m_Input->Initialize();

	//Create the graphics object
	m_Graphics = new GraphicsClass();
	if (!m_Graphics)
		return false;

	//Initialize the graphics object
	result = m_Graphics->Initialize(screenHeight, screenWidth, m_hwnd);
	if (!result)
		return false;

	CreateConsole();

	return true;
}

void SystemClass::Shutdown()
{
	//Release the input object
	if (m_Input)
	{
		delete m_Input;
		m_Input = nullptr;
	}

	//Release the graphics object
	if (m_Graphics)
	{
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	//Shutdown the window
	ShutdownWindows();

	return;
}

void SystemClass::Run()
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

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		//Check if a key has been pressed on the keyboard
		case WM_KEYDOWN:
		{
			//If a key is pressed send it to input object so it can record that state
			m_Input->KeyDown((unsigned int)wparam);
			break;
		}

		//Check if a key has been released on the keyboard
		case WM_KEYUP:
		{
			//If a key has been released then send it to the input object so it can unset the state for that key
			m_Input->KeyUp((unsigned int)wparam);
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

bool SystemClass::Frame()
{
	bool result;

	//Check if the user pressed escape and wants to exit the application
	if (m_Input->IsKeyDown(VK_ESCAPE))
		return false;

	//Do the frame processing for the graphics object
	result = m_Graphics->Frame();
	if (!result)
		return false;

	return true;
}

void SystemClass::InitializeWindows(int& screenHeight, int& screenWidth)
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
		screenWidth = 800;
		screenHeight = 600;

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

void SystemClass::ShutdownWindows()
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

void SystemClass::CreateConsole()
{
	HANDLE hStdHandle;
	FILE* fp = nullptr;

	//Allocate a console
	AllocConsole();

	//Redirect unbuffered STDOUT to the console
	hStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	setvbuf(stdout, NULL, _IONBF, 0);
	
	//Redirect unbuffered STDIN to the console
	hStdHandle = GetStdHandle(STD_INPUT_HANDLE);
	freopen_s(&fp, "CONIN$", "r", stdin);
	setvbuf(stdin, NULL, _IONBF, 0);
	
	//Redirect unbuffered STDERR to the console
	hStdHandle = GetStdHandle(STD_ERROR_HANDLE);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	setvbuf(stderr, NULL, _IONBF, 0);

	//Make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog 
	//point to console as well
	std::ios::sync_with_stdio();

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

		//All other messages pass to the message handler in the system class
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}

	return 0;
}
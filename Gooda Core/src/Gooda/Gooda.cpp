#include "Gooda.h"
#include "Console.h"
#include "Input.h"

void Gooda::Release()
{
	if (this)
		delete this;
}

bool Gooda::Frame()
{
	return true;
}

LRESULT Gooda::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	//Check if a key has been pressed on the keyboard
	case WM_KEYDOWN:
	{
		//If a key is pressed send it to input object so it can record that state
		GoodaCore::Input::Instance()->ProcessKeyboardMessage(umsg, wparam, lparam);

		break;
	}

	//Check if a key has been released on the keyboard
	case WM_KEYUP:
	{
		//If a key has been released then send it to the input object so it can unset the state for that key
		GoodaCore::Input::Instance()->ProcessKeyboardMessage(umsg, wparam, lparam);
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
		//If mouse was moved send to input object to update the mouse
		GoodaCore::Input::Instance()->ProcessMouseMessage(umsg, wparam, lparam);
		break;
	}

	//Any other message send to the default message handler as our application won't use them
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}

	return 0;
}

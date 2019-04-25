// Gooda.cpp : Defines the functions for the static library.
//
#include <Gooda.h>
#include "..\..\Gooda Core\src\Gooda\Input.h"

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

	case WM_MOUSEMOVE:
	{
		//If mouse was moved send to input object to update the camera
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

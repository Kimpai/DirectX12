#include "Input.h"

namespace GoodaCore
{
	Input* Input::Instance()
	{
		static Input s_input;
		return &s_input;
	}

	void Input::ProcessKeyboardMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		m_keyboard->ProcessMessage(message, wParam, lParam);
	}

	void Input::ProcessMouseMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		m_mouse->ProcessMessage(message, wParam, lParam);
	}

	bool Input::IsKeyPressed(unsigned int key)
	{
		return m_keyboardTracker->IsKeyPressed((Keyboard::Keys)key);
	}

	bool Input::IsKeyReleased(unsigned int key)
	{
		return m_keyboardTracker->IsKeyReleased((Keyboard::Keys)key);
	}

	bool Input::IsKeyDown(unsigned int key)
	{
		return m_keyboard->GetState().IsKeyDown((Keyboard::Keys)key);
	}

	XMFLOAT2 Input::GetMousePosition()
	{
		return XMFLOAT2(m_mouseX, m_mouseY);
	}

	void Input::SetMouseInputMode(Mouse::Mode mode)
	{
		m_mouse->SetMode(mode);
	}

	void Input::SetMouseInputWindow(HWND hwnd)
	{
		m_mouse->SetWindow(hwnd);
	}

	bool Input::Init(HWND hwnd)
	{
		m_mouseX = 0.0f;
		m_mouseY = 0.0f;

		//Create keyboard object
		m_keyboard = std::make_unique<Keyboard>();

		//Create mouse object
		m_mouse = std::make_unique<Mouse>();
		m_mouse->SetWindow(hwnd);
		m_mouse->SetMode(Mouse::MODE_RELATIVE);

		//Create a keyboard state tracker
		m_keyboardTracker = std::make_unique<Keyboard::KeyboardStateTracker>();

		//Create a mouse state tracker
		m_mouseTracker = std::make_unique<Mouse::ButtonStateTracker>();

		return true;
	}

	bool Input::Destroy()
	{
		m_mouse = nullptr;
		m_keyboard = nullptr;
		m_keyboardTracker = nullptr;
		m_mouseTracker = nullptr;

		return true;
	}

	bool Input::Frame()
	{
		//Update the keyboard state
		m_keyboardTracker->Update(m_keyboard->GetState());

		//Update the mouse state
		m_mouseTracker->Update(m_mouse->GetState());

		//Update the location of the mouse
		m_mouseX = (float)m_mouseTracker->GetLastState().x;
		m_mouseY = (float)m_mouseTracker->GetLastState().y;

		return true;
	}
}
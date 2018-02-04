#include "Input.h"

Input::Input()
{
	m_mouse = nullptr;
	m_keyboard = nullptr;
	m_keyboardTracker = nullptr;
	m_mouseTracker = nullptr;
	m_mouseX = 0.0f;
	m_mouseY = 0.0f;
}

Input::Input(const Input& other)
{

}

Input::~Input()
{
	m_mouse = nullptr;
	m_keyboard = nullptr;
	m_keyboardTracker = nullptr;
	m_mouseTracker = nullptr;
}

void Input::Initialize(HWND hwnd)
{
	//Create keyboard object
	m_keyboard = std::make_unique<Keyboard>();

	//Create mouse object
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(hwnd);
	m_mouse->SetMode(Mouse::MODE_ABSOLUTE);

	//Create a keyboard state tracker
	m_keyboardTracker = std::make_unique<Keyboard::KeyboardStateTracker>();

	//Create a mouse state tracker
	m_mouseTracker = std::make_unique<Mouse::ButtonStateTracker>();

	return;
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

void Input::Frame()
{
	//Update the keyboard state
	m_keyboardTracker->Update(m_keyboard->GetState());

	//Update the mouse state
	m_mouseTracker->Update(m_mouse->GetState());

	//Update the location of the mouse
	m_mouseX += (float)m_mouse->GetState().x;
	m_mouseY += (float)m_mouse->GetState().y;

	//Ensure the mouse location doesn't exceed the screen width and height
	if (m_mouseX < 0.0f)
		m_mouseX = 0.0f;

	if (m_mouseY < 0.0f)
		m_mouseY = 0.0f;

	if (m_mouseX > SCREEN_WIDTH)
		m_mouseX = SCREEN_WIDTH;

	if (m_mouseY > SCREEN_HEIGHT)
		m_mouseY = SCREEN_HEIGHT;
}

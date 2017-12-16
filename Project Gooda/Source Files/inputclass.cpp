#include "inputclass.h"

Input::Input()
{
	m_mouse = nullptr;
	m_keyboard = nullptr;
}

Input::Input(const Input& other)
{

}

Input::~Input()
{

}

void Input::Initialize(HWND hwnd)
{
	//Create keyboard object
	m_keyboard = std::make_unique<Keyboard>();

	//Create mouse object
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(hwnd);

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
	/*Keyboard::KeyboardStateTracker tracker;
	tracker.Update(m_keyboard->GetState());

	return tracker.IsKeyPressed((Keyboard::Keys)key);*/
	return false;
}

bool Input::IsKeyReleased(unsigned int key)
{
	/*Keyboard::KeyboardStateTracker tracker;
	tracker.Update(m_keyboard->GetState());

	return tracker.IsKeyReleased((Keyboard::Keys)key);*/
	return true;
}

bool Input::IsKeyDown(unsigned int key)
{
	//Return what state the key is in
	//return m_keyboard->GetState().IsKeyDown((Keyboard::Keys)key);
	return true;
}

XMFLOAT2 Input::GetMousePosition()
{
	//return XMFLOAT2((float)m_mouse->GetState().x, (float)m_mouse->GetState().y);
	return XMFLOAT2(0.0f, 0.0f);
}

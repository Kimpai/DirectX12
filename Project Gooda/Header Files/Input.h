#pragma once

#define VK_W	0x57
#define VK_A	0x41
#define VK_S	0x53
#define VK_D	0x44

#include <Windows.h>
#include <Mouse.h>
#include <DirectXMath.h>
#include <Keyboard.h>
#include <config.h>

using namespace DirectX;

class Input
{
public:
	Input();
	Input(const Input&);
	~Input();

	void Initialize(HWND);
	void ProcessKeyboardMessage(UINT, WPARAM, LPARAM);
	void ProcessMouseMessage(UINT, WPARAM, LPARAM);
	bool IsKeyPressed(unsigned int);
	bool IsKeyReleased(unsigned int);
	bool IsKeyDown(unsigned int);
	XMFLOAT2 GetMousePosition();
	void SetMouseInputMode(Mouse::Mode);
	void Frame();

private:
	std::unique_ptr<Keyboard> m_keyboard;
	std::unique_ptr<Keyboard::KeyboardStateTracker> m_keyboardTracker;
	std::unique_ptr<Mouse> m_mouse;
	std::unique_ptr<Mouse::ButtonStateTracker> m_mouseTracker;

	float m_mouseX, m_mouseY;
	
};
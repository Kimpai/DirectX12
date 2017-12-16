#pragma once

#include <Windows.h>
#include <Mouse.h>
#include <DirectXMath.h>
#include <Keyboard.h>

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

private:
	std::unique_ptr<Keyboard> m_keyboard;
	std::unique_ptr<Mouse> m_mouse;
	
};
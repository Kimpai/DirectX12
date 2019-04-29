#pragma once

#define VK_W	0x57
#define VK_A	0x41
#define VK_S	0x53
#define VK_D	0x44

#include <Windows.h>
#include <DirectXMath.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <iostream>

#include "Gooda.h"

using namespace DirectX;

namespace GoodaCore
{
	class Input
	{
	public:
		virtual ~Input() = default;

		static Input* Instance();

		void ProcessKeyboardMessage(UINT, WPARAM, LPARAM);
		void ProcessMouseMessage(UINT, WPARAM, LPARAM);
		bool IsKeyPressed(unsigned int);
		bool IsKeyReleased(unsigned int);
		bool IsKeyDown(unsigned int);
		XMFLOAT2 GetMousePosition();
		void SetMouseInputMode(Mouse::Mode);
		void SetMouseInputWindow(HWND);

		virtual bool Init(HWND);
		virtual bool Destroy();
		virtual bool Frame();

	private:
		Input() = default;

		std::unique_ptr<Keyboard> m_keyboard;
		std::unique_ptr<Keyboard::KeyboardStateTracker> m_keyboardTracker;
		std::unique_ptr<Mouse> m_mouse;
		std::unique_ptr<Mouse::ButtonStateTracker> m_mouseTracker;

		float m_mouseX, m_mouseY;
	};
}
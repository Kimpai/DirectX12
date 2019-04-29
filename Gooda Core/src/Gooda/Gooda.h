#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

class Gooda
{
public:
	virtual ~Gooda() = default;

	virtual bool Frame();
	void Release();

	static LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

protected:
	Gooda() = default;

	bool m_fullScreen = false;
	bool m_vsync = true;
	float m_screenDepth = 1000.0f;
	float m_screenNear = 1.0f;
	unsigned int m_screenWidth = 800;
	unsigned int m_screenHeight = 600;
};
#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <Gooda.h>
#include <fstream>
#include "Input.h"
#include "GoodaDriver.h"
#include "Console.h"
#include "Camera.h"

class GoodaDevice : public Gooda
{
public:
	GoodaDevice();
	~GoodaDevice();

	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void SetupWindows(int&, int&);

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	Input* m_inputDevice;
	GoodaDriver* m_driver;
	Console* m_console;
	Camera* m_camera;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static GoodaDevice* ApplicationHandle = nullptr;
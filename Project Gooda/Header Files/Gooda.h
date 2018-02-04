#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <config.h>
#include <fstream>
#include "Input.h"
#include "GoodaDriver.h"
#include "Console.h"
#include "LuaScript.h"
#include "Camera.h"

class GoodaDevice
{
public:
	GoodaDevice();
	GoodaDevice(const GoodaDevice&);
	~GoodaDevice();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	Input* m_Input;
	GoodaDriver* m_Driver;
	Console* m_Console;
	Lua* m_Lua;
	Camera* m_Camera;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static GoodaDevice* ApplicationHandle = nullptr;
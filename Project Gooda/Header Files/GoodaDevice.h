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

	void SetRenderWindow(unsigned int screenHeight, unsigned int screenWidth, float screenDepth, float screenNear);
	void EnableVSYNC(bool vsyncEnabled);
	void SetFullScreen(bool fullScreen);

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

private:
	bool Frame();
	void SetupWindows();

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	Input* m_inputDevice;
	GoodaDriver* m_driver;
	Console* m_console;
	Camera* m_camera;
};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
static GoodaDevice* ApplicationHandle = nullptr;
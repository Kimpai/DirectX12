#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>

// The min/max macros conflict with like-named member functions.
// Only use std::min and std::max defined in <algorithm>.
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

// In order to define a function called CreateWindow, the Windows macro needs to
// be undefined.
#if defined(CreateWindow)
#undef CreateWindow
#endif

class Gooda
{
public:
	virtual ~Gooda() = default;

	virtual bool Frame();
	void Release();

protected:
	Gooda() = default;

	bool m_fullScreen = false;
	bool m_vsync = true;
	float m_screenDepth = 1000.0f;
	float m_screenNear = 1.0f;
	unsigned int m_screenWidth = 800;
	unsigned int m_screenHeight = 600;
};
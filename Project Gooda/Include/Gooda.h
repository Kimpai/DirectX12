#pragma once

class Gooda
{
public:
    Gooda();
    virtual ~Gooda() = 0;

protected:
	bool m_fullScreen = false;
	bool m_vsync = true;
	float m_screenDepth = 1000.0f;
	float m_screenNear = 0.1f;
	unsigned int m_screenWidth = 800;
	unsigned int m_screenHeight = 600;
};


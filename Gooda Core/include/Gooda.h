#pragma once

class Gooda
{
public:
	inline Gooda();
	inline virtual ~Gooda() = 0;

	inline void Release();


protected:
	bool m_fullScreen = false;
	bool m_vsync = true;
	float m_screenDepth = 1000.0f;
	float m_screenNear = 0.1f;
	unsigned int m_screenWidth = 800;
	unsigned int m_screenHeight = 600;
};

inline Gooda::Gooda()
{

}

inline Gooda::~Gooda()
{

}

inline void Gooda::Release()
{
	if (this)
		delete this;
}
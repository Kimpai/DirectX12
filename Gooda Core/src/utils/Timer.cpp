#include "Timer.h"

Timer::Timer()
{
	m_frameCounter = 0;
	m_elapsedSeconds = 0.0f;
	m_time0 = m_clock.now();
	m_time1 = m_time0;
	m_deltaTime = m_time1 - m_time0;
}

Timer* Timer::Instance()
{
	static Timer s_timer;
	return &s_timer;
}

void Timer::FPS()
{
	m_frameCounter++;
	m_time1 = m_clock.now();
	m_deltaTime = m_time1 - m_time0;

	m_elapsedSeconds += m_deltaTime.count() * 1e-9;
	if (m_elapsedSeconds > 1.0)
	{
		char buffer[500];
		auto fps = m_frameCounter / m_elapsedSeconds;
		sprintf_s(buffer, 500, "FPS: %f\n", fps);
		OutputDebugString((LPCWSTR)buffer);

		m_frameCounter = 0;
		m_elapsedSeconds = 0.0;
		m_time0 = m_clock.now();
	}
}
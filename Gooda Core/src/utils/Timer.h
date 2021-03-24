#pragma once
#include <chrono>
#include <Windows.h>

class Timer
{
public:
	virtual ~Timer() = default;
	static Timer* Instance();

	void FPS();

private:
	Timer();

	UINT64 m_frameCounter;
	double m_elapsedSeconds;
	std::chrono::high_resolution_clock m_clock;
	std::chrono::steady_clock::time_point m_time0;
	std::chrono::steady_clock::time_point m_time1;
	std::chrono::steady_clock::duration m_deltaTime;
};
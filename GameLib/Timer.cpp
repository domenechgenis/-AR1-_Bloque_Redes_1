#include "Timer.h"

Timer::Timer()
{
	m_start = std::chrono::system_clock::now();
}

Timer::~Timer()
{
}


void Timer::ResetTimer()
{
	m_start = std::chrono::system_clock::now();
}

unsigned int Timer::GetDuration()
{
	m_end = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli> duration = m_end - m_start;
	return duration.count() * std::chrono::milliseconds::period::num / std::chrono::milliseconds::period::den;
}
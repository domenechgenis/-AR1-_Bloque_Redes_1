#pragma once
#include <iostream>
#include <chrono>

class Timer
{
private:
	std::chrono::system_clock::time_point m_start;
	std::chrono::system_clock::time_point m_end;
public:
	Timer();
	~Timer();

	//Utils functions
	void ResetTimer();
	unsigned int GetDuration();
};
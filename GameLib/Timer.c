
#pragma once
#include <iostream>
#include <chrono>

class Timer
{
private:
	std::chrono::system_clock::time_point m_start;
	std::chrono::system_clock::time_point m_end;

	int server_desconnected_times = 0;

public:
	Timer();
	~Timer();

	//Utils functions
	void ResetTimer();
	unsigned int GetDuration();

	float GetMilisDuration();

	int GetServerDisconnected();
	void SetServerDisconnected(int);

};
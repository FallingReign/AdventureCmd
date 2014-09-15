#include "Timer.hpp"
#include <chrono>

using namespace std::chrono;

Timer::Timer()
{
    auto now = high_resolution_clock::now();
    m_start = duration_cast<milliseconds>(now.time_since_epoch()).count();
}

unsigned int Timer::Restart()
{
    auto now = high_resolution_clock::now();
    unsigned int timeNow = duration_cast<milliseconds>(now.time_since_epoch()).count();
    unsigned int elapsed = timeNow - m_start;
    m_start = timeNow;
    return elapsed;
}
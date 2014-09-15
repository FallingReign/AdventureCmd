#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

class Timer
{
public:
    Timer();

    unsigned int Restart();
private:
    unsigned int m_start;
};

#endif
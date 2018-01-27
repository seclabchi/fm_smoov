#include "logger.h"

#include <iostream>

using namespace Tonekids;

Logger::Logger(string name)
{
    m_raw_time = new time_t;
    m_time = new struct tm;
    m_name = new string(name);
}

Logger::~Logger()
{
    delete m_name;
}

void Logger::log_msg(string msg)
{
    time(m_raw_time);
    cout << *m_raw_time << "\t" << msg << endl;
}

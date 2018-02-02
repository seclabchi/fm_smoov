#include "logger.h"

#include <iostream>
#include <sstream>
#include <algorithm>

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
    replace(msg.begin(), msg.end(), '\n', ' ');
    replace(msg.begin(), msg.end(), '\r', ' ');
    m_logmsg_ss.str(string());
    time(m_raw_time);
    m_logmsg_ss << *m_raw_time << "\t" << msg << endl;
    cout << m_logmsg_ss.str();
    m_logmsg_cb(m_logmsg_ss.str());
}

void Logger::log_msg(stringstream msgstream)
{
    this->log_msg(msgstream.str());
}

void Logger::log_msg(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(m_msgbuf, format, args);
    va_end(args);
    
    this->log_msg(string(m_msgbuf));
}


void Logger::set_ui_logmsg_callback(LOGMSG_UI_CALLBACK cb)
{
    m_logmsg_cb = cb;
}

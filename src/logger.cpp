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

string Logger::log_level_to_string(LogLevel lvl)
{
    switch(lvl)
    {
        case DEBUG_LOW:
            return string("DEBUG_LOW");
            break;
        case DEBUG:
            return string("DEBUG");
            break;
        case DEBUG_HIGH:
            return string("DEBUG_HIGH");
            break;
        case INFO:
            return string("INFO");
            break;
        case WARN:
            return string("WARN");
            break;
        case ERROR:
            return string("ERROR");
            break;
        case CRITICAL:
            return string("CRITICAL");
            break;
        default:
            return string("UNKNOWN");
            break;
        
    }
}

void Logger::log_msg(LogLevel lvl, string msg)
{
    replace(msg.begin(), msg.end(), '\n', ' ');
    replace(msg.begin(), msg.end(), '\r', ' ');
    m_logmsg_ss.str(string());
    time(m_raw_time);
    m_logmsg_ss << *m_raw_time << "\t" << *(this->m_name) << "\t" << log_level_to_string(lvl) << "\t" << msg << endl;
    cout << m_logmsg_ss.str();
    
    /* TODO: completely hokey - can cause catastrophic side effects */
    m_logmsg_cb(lvl, m_logmsg_ss.str());  
}

void Logger::log_msg(LogLevel lvl, stringstream msgstream)
{
    this->log_msg(lvl, msgstream.str());
}

void Logger::log_msg(LogLevel lvl, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(m_msgbuf, format, args);
    va_end(args);
    
    this->log_msg(lvl, string(m_msgbuf));
}


void Logger::set_ui_logmsg_callback(LOGMSG_UI_CALLBACK cb)
{
    m_logmsg_cb = cb;
}

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>
#include <time.h>
#include <stdarg.h>

#include <sstream>

#define LOG_MSG_MAX_LEN_CHARS 4096

using namespace std;

namespace Tonekids
{

typedef enum 
{
    ALL,
    DEBUG_LOW,
    DEBUG,
    DEBUG_HIGH,
    INFO,
    WARN,
    ERROR,
    CRITICAL,
    NONE
} LogLevel;

typedef void (*LOGMSG_UI_CALLBACK)(LogLevel, string);

class Logger
{
public:
    Logger(string name);
    virtual ~Logger();
    virtual void log_msg(LogLevel lvl, string msg);
    virtual void log_msg(LogLevel lvl, stringstream msgstream);
    virtual void log_msg(LogLevel lvl, const char* format, ...);
    void set_ui_logmsg_callback(LOGMSG_UI_CALLBACK cb);
private:
    Logger() {};
    string log_level_to_string(LogLevel lvl);
    char m_msgbuf[LOG_MSG_MAX_LEN_CHARS];
    time_t* m_raw_time;
    struct tm* m_time;
    string* m_name;
    stringstream m_logmsg_ss;
    LOGMSG_UI_CALLBACK m_logmsg_cb;
};

}

#endif
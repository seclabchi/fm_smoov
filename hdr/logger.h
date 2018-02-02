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
    
typedef void (*LOGMSG_UI_CALLBACK)(string);

enum LogLevel
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
};

class Logger
{
public:
    Logger(string name);
    virtual ~Logger();
    virtual void log_msg(string msg);
    virtual void log_msg(stringstream msgstream);
    virtual void log_msg(const char* format, ...);
    void set_ui_logmsg_callback(LOGMSG_UI_CALLBACK cb);
private:
    Logger() {};
    char m_msgbuf[LOG_MSG_MAX_LEN_CHARS];
    time_t* m_raw_time;
    struct tm* m_time;
    string* m_name;
    stringstream m_logmsg_ss;
    LOGMSG_UI_CALLBACK m_logmsg_cb;
};

}

#endif
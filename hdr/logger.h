#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>
#include <time.h>

using namespace std;

class Logger
{
public:
    Logger(string name);
    virtual ~Logger();
    virtual void log_msg(string msg);
private:
    time_t* m_raw_time;
    struct tm* m_time;
    string* m_name;
};

#endif
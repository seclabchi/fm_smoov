#ifndef __PCM_DEVICE_H__
#define __PCM_DEVICE_H__

#include <string>

#include <asoundlib.h>

using namespace std;

class PCM_Device
{
public:
    PCM_Device(string name);
    virtual void configure() = 0;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
private:
    string* m_name;
    snd_pcm_t* m_handle;
};

#endif
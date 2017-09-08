#ifndef __PCM_DEVICE_H__
#define __PCM_DEVICE_H__

#include <string>
#include <asoundlib.h>

using namespace std;

class PCM_Device
{
public:
    PCM_Device(string* name);
    void open();
    void start();
    void stop();
    void close();
    virtual ~PCM_Device();
private:
    string* m_name;
    snd_pcm_t* mh_cap;
    snd_pcm_t* mh_pb;
    
    void configure(snd_pcm_t* handle, string subdev_name);
    
    static const snd_pcm_uframes_t BUFFER_SIZE_REQ = 48000;
    static const snd_pcm_uframes_t PERIOD_SIZE_REQ = 4800;
};

#endif
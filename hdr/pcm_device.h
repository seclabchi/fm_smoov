#ifndef __PCM_DEVICE_H__
#define __PCM_DEVICE_H__

#include <string>
#include <asoundlib.h>

#include "pcm_playback.h"
#include "pcm_capture.h"
#include "audio_hub.h"

using namespace std;

class PCM_Device
{
public:
    PCM_Device(string* name);
    void open();
    void start();
    void stop();
    void close();
    void set_audio_hub(AudioHub* audio_hub);
    virtual ~PCM_Device();
private:
    string* m_name;
    snd_pcm_t* mh_cap;
    snd_pcm_t* mh_pb;
    
    AudioHub* m_audio_hub;
    
    PCM_Playback* m_pcm_pb;
    PCM_Capture* m_pcm_cap;
    
    void configure(snd_pcm_t* handle, string subdev_name);
    
    static const snd_pcm_uframes_t BUFFER_SIZE_REQ = 8192;
    static const snd_pcm_uframes_t PERIOD_SIZE_REQ = 4096;
    
    snd_pcm_uframes_t m_bufsize_pb;
    snd_pcm_uframes_t m_persize_pb;
    snd_pcm_uframes_t m_bufsize_cap;
    snd_pcm_uframes_t m_persize_cap;
};

#endif
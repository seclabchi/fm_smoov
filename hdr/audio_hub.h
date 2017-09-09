#ifndef __AUDIO_HUB_H__
#define __AUDIO_HUB_H__

#include <stdint.h>
#include <semaphore.h>
#include <asoundlib.h>

#include "pcm_transfer_interface.h"

class AudioHub : public PCM_Transfer_Interface
{
public:
    AudioHub(snd_pcm_uframes_t bufsize_cap, snd_pcm_uframes_t persize_cap, snd_pcm_uframes_t bufsize_pb, snd_pcm_uframes_t persize_pb);
    virtual ~AudioHub();
    virtual void write_buffer(const void* bufsrc, size_t size, size_t count);
    virtual void read_buffer(void** bufdst, size_t size, size_t count);
    virtual uint32_t get_buffer_size_bytes();
    virtual uint8_t get_frame_size_bytes();
    virtual uint8_t get_buffer_size_frames();
    virtual uint8_t get_channels();
private:
    snd_pcm_uframes_t m_bufsize_cap;
    snd_pcm_uframes_t m_persize_cap;
    snd_pcm_uframes_t m_bufsize_pb;
    snd_pcm_uframes_t m_persize_pb;
    int16_t** cap_bufs;
    uint8_t cap_buf_index;
    
    sem_t m_buf_sem;
    
    int16_t* m_buf_main;
    
    FILE* m_cap_file;
};

#endif
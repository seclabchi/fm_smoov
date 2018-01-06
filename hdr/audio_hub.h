#ifndef __AUDIO_HUB_H__
#define __AUDIO_HUB_H__

#include <stdint.h>
#include <semaphore.h>
#include <asoundlib.h>
#include <vector>

#include "audio_params.h"
#include "pcm_transfer_interface.h"
#include "processor.h"
#include "processor_simple_gain.h"

using namespace std;

#define NUM_AUDIO_BUFS 2

class AudioHub : public PCM_Transfer_Interface
{
public:
    AudioHub();
    virtual ~AudioHub();
    virtual void configure(snd_pcm_uframes_t bufsize_cap, snd_pcm_uframes_t persize_cap, snd_pcm_uframes_t bufsize_pb, snd_pcm_uframes_t persize_pb);
    virtual void write_buffer(const void* bufsrc, size_t size, size_t count);
    virtual void read_buffer(void** bufdst, size_t size, size_t count);
    virtual uint32_t get_buffer_size_bytes();
    virtual uint8_t get_frame_size_bytes();
    virtual uint8_t get_buffer_size_frames();
    virtual uint8_t get_channels();
    
    virtual void add_processor(Processor* p);
private:
    snd_pcm_uframes_t m_bufsize_cap;
    snd_pcm_uframes_t m_persize_cap;
    snd_pcm_uframes_t m_bufsize_pb;
    snd_pcm_uframes_t m_persize_pb;
    float** cap_bufs;
    uint8_t cap_buf_index;
    
    audio_params_t m_audio_params;
    
    sem_t m_buf_sem;
    
    float** m_buf_main;
    uint8_t m_buf_index_read;
    uint8_t m_buf_index_write;
    
    int32_t m_frame_delta;
    
    FILE* m_cap_file;
    vector<Processor*>* m_pchain;
    vector<Processor*>::iterator m_pchain_it;
};

#endif
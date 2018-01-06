#include "audio_hub.h"

#include <iostream>

using namespace std;

AudioHub::AudioHub()
{
    sem_init(&m_buf_sem, 0, 1);
    m_cap_file = fopen("/tmp/audiod_cap.pcm", "wb");
    m_buf_index_read = 0;
    m_buf_index_write = 0;
    m_pchain = new vector<Processor*>();
    m_buf_main = new float*[NUM_AUDIO_BUFS];
}

AudioHub::~AudioHub()
{
    sem_destroy(&m_buf_sem);
    fclose(m_cap_file);
    
    for(int i = 0; i < NUM_AUDIO_BUFS; i++)
    {
        delete[] m_buf_main[i];
    }
    
    delete[] m_buf_main;
    
    vector<Processor*>::iterator it;
    
    for(it = m_pchain->begin(); it != m_pchain->end(); it++)
    {
        delete *it;
    }
    
    delete m_pchain;
}

void AudioHub::add_processor(Processor* p)
{
    m_pchain->push_back(p);
}

void AudioHub::write_buffer(const void* bufsrc, size_t size, size_t count)
{
    sem_wait(&m_buf_sem);
    cout << "write_buffer ENTER" << endl;
    //fwrite(bufsrc, size, count, m_cap_file);
    
    memcpy(m_buf_main[m_buf_index_write % NUM_AUDIO_BUFS], bufsrc, size*count);
    
    for(m_pchain_it = m_pchain->begin(); m_pchain_it != m_pchain->end(); m_pchain_it++)
    {
        (*m_pchain_it)->process((float*)(m_buf_main[m_buf_index_write % NUM_AUDIO_BUFS]), count/2);
    }
    
    m_buf_index_write++;
    m_frame_delta++;
    cout << "Frame delta is " << m_frame_delta << endl;
    //cout << "write_buffer EXIT" << endl;
    //sem_post(&m_buf_sem);
}

void AudioHub::read_buffer(void** bufdst, size_t size, size_t count)
{
    //sem_wait(&m_buf_sem);
    //cout << "read_buffer ENTER" << endl;
    m_frame_delta--;
    memcpy(*bufdst, m_buf_main[m_buf_index_read % NUM_AUDIO_BUFS], size*count);
    m_buf_index_read++;
    //cout << "read_buffer EXIT - delta = " << m_frame_delta << endl;
 
    sem_post(&m_buf_sem);
}

uint32_t AudioHub::get_buffer_size_bytes()
{
    return 0;
}

uint8_t AudioHub::get_frame_size_bytes()
{
    return 0;
}

uint8_t AudioHub::get_buffer_size_frames()
{
    return 0;
}

uint8_t AudioHub::get_channels()
{
    return 0;
}

void AudioHub::configure(snd_pcm_uframes_t bufsize_cap, snd_pcm_uframes_t persize_cap, snd_pcm_uframes_t bufsize_pb, snd_pcm_uframes_t persize_pb)
{
    m_bufsize_cap = bufsize_cap;
    m_persize_cap = persize_cap;
    m_bufsize_pb = bufsize_pb;
    m_persize_pb = persize_pb;
    
    m_buf_index_read = 0;
    m_buf_index_write = 0;
    
    for(int i = 0; i < NUM_AUDIO_BUFS; i++)
    {
        m_buf_main[i] = new float[bufsize_cap * 2];
        memset(m_buf_main[i], 0, bufsize_cap*2*sizeof(int16_t));
    }
    
    m_frame_delta = 0;
    
    m_audio_params.samp_rate = 48000;
    m_audio_params.num_chans = 2;
}
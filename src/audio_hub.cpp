#include "audio_hub.h"

#include <iostream>

using namespace std;

AudioHub::AudioHub()
{
    sem_init(&m_buf_sem, 0, 1);
    m_cap_file = fopen("/tmp/audiod_cap.pcm", "wb");
    
    m_pchain = new vector<Processor*>();
}

AudioHub::~AudioHub()
{
    sem_destroy(&m_buf_sem);
    fclose(m_cap_file);
    delete[] m_buf_main[0];
    delete[] m_buf_main[1];
    
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
    //cout << "write_buffer ENTER" << endl;
    //fwrite(bufsrc, size, count, m_cap_file);
    memcpy(m_buf_main[m_buf_index_write], bufsrc, size*count);
    
    for(m_pchain_it = m_pchain->begin(); m_pchain_it != m_pchain->end(); m_pchain_it++)
    {
        (*m_pchain_it)->process((void*)(m_buf_main[m_buf_index_write]), size, count);
    }
    
    m_buf_index_write == 0 ? m_buf_index_write = 1 : m_buf_index_write = 0;
    m_frame_delta++;
    cout << "Frame delta is " << m_frame_delta << endl;
    //cout << "write_buffer EXIT" << endl;
    sem_post(&m_buf_sem);
}

void AudioHub::read_buffer(void** bufdst, size_t size, size_t count)
{
    sem_wait(&m_buf_sem);
    //cout << "read_buffer ENTER" << endl;
    m_frame_delta--;
    memcpy(*bufdst, m_buf_main[m_buf_index_read], size*count);
    m_buf_index_read == 0 ? m_buf_index_read = 1 : m_buf_index_read = 0;
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
    m_buf_main[0] = new int16_t[bufsize_cap * 2];
    memset(m_buf_main[0], 0, bufsize_cap*2*sizeof(int16_t));
    m_buf_main[1] = new int16_t[bufsize_cap * 2];
    memset(m_buf_main[1], 0, bufsize_cap*2*sizeof(int16_t));
    m_frame_delta = 0;
}
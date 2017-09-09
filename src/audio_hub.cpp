#include "audio_hub.h"

AudioHub::AudioHub(snd_pcm_uframes_t bufsize_cap, snd_pcm_uframes_t persize_cap, snd_pcm_uframes_t bufsize_pb, snd_pcm_uframes_t persize_pb)
{
    sem_init(&m_buf_sem, 0, 1);
    m_cap_file = fopen("/tmp/audiod_cap.pcm", "wb");
    m_bufsize_cap = bufsize_cap;
    m_persize_cap = persize_cap;
    m_bufsize_pb = bufsize_pb;
    m_persize_pb = persize_pb;
    
    m_buf_main = new int16_t[bufsize_cap * 2];
    
}

AudioHub::~AudioHub()
{
    sem_destroy(&m_buf_sem);
    fclose(m_cap_file);
    delete[] m_buf_main;
}

void AudioHub::write_buffer(const void* bufsrc, size_t size, size_t count)
{
    sem_wait(&m_buf_sem);
    //fwrite(bufsrc, size, count, m_cap_file);
    memcpy(m_buf_main, bufsrc, size*count);
    sem_post(&m_buf_sem);
}

void AudioHub::read_buffer(void** bufdst, size_t size, size_t count)
{
    sem_wait(&m_buf_sem);
    memcpy(*bufdst, m_buf_main, size*count);
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
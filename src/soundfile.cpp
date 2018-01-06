#include "soundfile.h"
#include <string.h>

SoundFile::SoundFile(string filename)
{
    m_filename = filename;
    memset(&m_file_info, 0, sizeof(m_file_info));
    m_file = sf_open(filename.c_str(), SFM_READ, &m_file_info);
    
    if(NULL == m_file)
    {
        throw runtime_error(sf_strerror(NULL));
    }
    
    m_frames_remain = m_file_info.frames;
}

uint64_t SoundFile::get_frames(uint64_t frame_count, float* buf)
{
    sf_count_t frames_read = 0;
    
    if(m_frames_remain >= frame_count)
    {
        frames_read = sf_readf_float(m_file, buf, frame_count);
        m_frames_remain -= frames_read;
    }
    else
    {
        frames_read = sf_readf_float(m_file, buf, m_frames_remain);
        m_frames_remain = 0;
    }
    
    return frames_read;
}

SoundFile::~SoundFile()
{
    if(NULL != m_file)
    {
        sf_close(m_file);
    }
}

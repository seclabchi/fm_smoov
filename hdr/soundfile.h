#ifndef __SOUNDFILE_OPS_H__
#define __SOUNDFILE_OPS_H__

#include <stdio.h>
#include <sndfile.h>
#include <stdexcept>
#include <string>

using namespace std;

class SoundFile
{
public:
    SoundFile(string filename);
    virtual ~SoundFile();
    uint64_t get_frames(uint64_t frame_count, float* buf);

private:
    SoundFile() {}
    string m_filename;
    SNDFILE* m_file;
    SF_INFO m_file_info;
    uint64_t m_frames_remain;
};

#endif //__SOUNDFILE_OPS_H__
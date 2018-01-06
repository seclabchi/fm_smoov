#ifndef __STREAM_CAPTURE_H__
#define __STREAM_CAPTURE_H__

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include "soundfile.h"
#include "pcm_transfer_interface.h"

using namespace std;

class StreamCapture
{
    public:
    StreamCapture(string filename, uint32_t framesize);
    virtual ~StreamCapture();
    void set_transfer_interface(PCM_Transfer_Interface* xfer_iface);
    void start();
    void* stop();
    static void* class_thread_run_thunk(void* args);
    void* thread_run_func(void* args);
private:
    void prepare_capture_loop();
    void run_capture_loop();
    pthread_t mh_thread;
    sem_t mh_sem_thread_start;
    bool m_should_stop;
    
    PCM_Transfer_Interface* m_xfer_iface;
    int channels = 2;
    float* buf_in;
    uint32_t m_framesize;
    SoundFile* m_sound_file;
    float m_sleep_dur;
};

#endif
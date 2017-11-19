#ifndef __PCM_PLAYBACK__H__
#define __PCM_PLAYBACK__H__

#include <pthread.h>
#include <semaphore.h>
#include <asoundlib.h>
#include <stdint.h>

#include "pcm_transfer_interface.h"

class PCM_Playback 
{
public:
    PCM_Playback(snd_pcm_t* pcm, snd_pcm_uframes_t bufsize, snd_pcm_uframes_t persize);
    virtual ~PCM_Playback();
    void set_transfer_interface(PCM_Transfer_Interface* xfer_iface);
    void start();
    void* stop();
    static void* class_thread_run_thunk(void* args);
    void* thread_run_func(void* args);
private:
    pthread_t mh_thread;
    sem_t mh_sem_thread_start;
    bool m_should_stop;
    
    snd_pcm_t* mh_pcm;
    snd_pcm_uframes_t m_bufsize;
    snd_pcm_uframes_t m_persize;
    struct pollfd *ufds;
    float *ptr;
    int err, count, cptr, init;
    int channels = 2;
    void prepare_playback_loop();
    int wait_for_poll();
    int xrun_recovery(int err);
    void run_playback_loop();
    
    PCM_Transfer_Interface* m_xfer_iface;
    
    float* buf_out;
};

#endif
#ifndef __PCM_CAPTURE__H__
#define __PCM_CAPTURE__H__

#include <pthread.h>
#include <semaphore.h>
#include <asoundlib.h>
#include <stdint.h>

class PCM_Capture 
{
public:
    PCM_Capture(snd_pcm_t* pcm, snd_pcm_uframes_t bufsize, snd_pcm_uframes_t persize);
    virtual ~PCM_Capture();
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
    signed short *ptr;
    int err, count, cptr, init;
    int channels = 2;
    void prepare_capture_loop();
    int wait_for_poll();
    int xrun_recovery(int err);
    void run_capture_loop();
    
    int16_t* buf_in;
    
    FILE* m_cap_file;
};

#endif
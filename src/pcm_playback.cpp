#include "pcm_playback.h"
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <string>

using namespace std;

PCM_Playback::PCM_Playback(snd_pcm_t* pcm, snd_pcm_uframes_t bufsize, snd_pcm_uframes_t persize)
{
    sem_init(&mh_sem_thread_start, 0, 1);
    mh_pcm = pcm;
    m_bufsize = bufsize;
    m_persize = persize;
    buf_out = new float[2*bufsize];
    memset(buf_out, 0, 2*bufsize*sizeof(float));
}

PCM_Playback::~PCM_Playback()
{
    sem_destroy(&mh_sem_thread_start);
    delete[] buf_out;
}

void PCM_Playback::set_transfer_interface(PCM_Transfer_Interface* xfer_iface)
{
    m_xfer_iface = xfer_iface;
}

void PCM_Playback::start()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    sem_wait(&mh_sem_thread_start);
    cout << "Starting playback thread..." << endl;
    pthread_create(&mh_thread, &attr, PCM_Playback::class_thread_run_thunk, (void*)this);
    sem_wait(&mh_sem_thread_start);
    cout << "Playback thread started." << endl;
    pthread_attr_destroy(&attr);
}

void* PCM_Playback::stop()
{
    cout << "Stopping playback thread..." << endl;
    void* retval = 0;
    this->m_should_stop = true;
    pthread_join(mh_thread, &retval);
    cout << "Playback thread stopped." << endl;
    return retval;
}

void* PCM_Playback::class_thread_run_thunk(void* args)
{
    PCM_Playback* obj = reinterpret_cast<PCM_Playback*>(args);
    return obj->thread_run_func(args);
}

void* PCM_Playback::thread_run_func(void* args)
{
    this->m_should_stop = false;
    sem_post(&mh_sem_thread_start);
    
    try
    {
        this->prepare_playback_loop();
        
        while(false == this->m_should_stop)
        {
            this->run_playback_loop();
        }
    }
    catch(std::runtime_error& excp)
    {
        cout << "A runtime exception was thrown from the playback loop: " << excp.what() << endl;
    }
    
    snd_pcm_drain(mh_pcm);
    
    cout << "Playback thread func exit." << endl;
    return NULL;
}

void PCM_Playback::prepare_playback_loop()
{
    
    count = snd_pcm_poll_descriptors_count (mh_pcm);
    if (count <= 0) {
            throw runtime_error("Invalid poll descriptors count: " + to_string(count));
            
    }
    ufds = (struct pollfd*)malloc(sizeof(struct pollfd) * count);
    if (ufds == (struct pollfd*)NULL) {
            throw runtime_error("Not enough memory");
    }
    if ((err = snd_pcm_poll_descriptors(mh_pcm, ufds, count)) < 0) {
            throw runtime_error(string("Unable to obtain poll descriptors for playback: ") + snd_strerror(err));
    }
    init = 1;
    ptr = buf_out;
}

int PCM_Playback::wait_for_poll()
{
    unsigned short revents;
    while (1) 
    {
        poll(ufds, count, -1);
        snd_pcm_poll_descriptors_revents(mh_pcm, ufds, count, &revents);
        if (revents & POLLERR)
                return -EIO;
        if (revents & POLLOUT)
                return 0;
    }
}

int PCM_Playback::xrun_recovery(int err)
{
        printf("Playback stream recovery\n");
        if (err == -EPIPE) {    /* under-run */
                err = snd_pcm_prepare(mh_pcm);
                if (err < 0)
                {
                    throw runtime_error(string("Can't recover from playback underrun, prepare failed: ") + snd_strerror(err));
                }
                return 0;
        } else if (err == -ESTRPIPE) {
                while ((err = snd_pcm_resume(mh_pcm)) == -EAGAIN)
                {
                    sleep(1);       /* wait until the suspend flag is released */
                }
                if (err < 0) 
                {
                    err = snd_pcm_prepare(mh_pcm);
                    if (err < 0)
                    {
                        throw runtime_error(string("Can't recover from playback suspend, prepare failed: ") + snd_strerror(err));
                    }
                }
                return 0;
        }
        return err;
}

void PCM_Playback::run_playback_loop()
{
    if (!init) {
            err = wait_for_poll();
            if (err < 0) {
                    if (snd_pcm_state(mh_pcm) == SND_PCM_STATE_XRUN ||
                        snd_pcm_state(mh_pcm) == SND_PCM_STATE_SUSPENDED) {
                            err = snd_pcm_state(mh_pcm) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
                            if (xrun_recovery(err) < 0) {
                                    throw runtime_error(string("Write error: ") + snd_strerror(err));
                            }
                            init = 1;
                    } else {
                            throw runtime_error("Wait for poll failed");
                    }
            }
    }
    
    if(ptr == (buf_out + (m_bufsize*2)))
    {
        m_xfer_iface->read_buffer((void**)&buf_out, sizeof(float), m_bufsize*2);
        ptr = buf_out;
    }
    cptr = m_persize;
    while (cptr > 0) 
    {
        err = snd_pcm_writei(mh_pcm, ptr, cptr);
        if (err < 0) 
        {
            if (xrun_recovery(err) < 0) 
            {
                throw runtime_error(string("Write error: ") + snd_strerror(err));
            }
            init = 1;
            break;  /* skip one period */
        }
        if (snd_pcm_state(mh_pcm) == SND_PCM_STATE_RUNNING)
                init = 0;
        ptr += err * channels;
        cptr -= err;
        if (cptr == 0)
                break;
        /* it is possible that the initial buffer cannot store */
        /* all data from the last period, so wait awhile */
        err = wait_for_poll();
        if (err < 0) 
        {
            if (snd_pcm_state(mh_pcm) == SND_PCM_STATE_XRUN ||
                snd_pcm_state(mh_pcm) == SND_PCM_STATE_SUSPENDED) 
            {
                err = snd_pcm_state(mh_pcm) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
                if (xrun_recovery(err) < 0) 
                {
                    throw runtime_error(string("Write error: ") + snd_strerror(err));
                }
                init = 1;
            } 
            else 
            {
                throw runtime_error("Wait for poll failed");
            }
        }
    }
}
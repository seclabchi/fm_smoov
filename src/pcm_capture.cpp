#include "pcm_capture.h"
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <string>

using namespace std;

PCM_Capture::PCM_Capture(snd_pcm_t* pcm, snd_pcm_uframes_t bufsize, snd_pcm_uframes_t persize)
{
    sem_init(&mh_sem_thread_start, 0, 1);
    mh_pcm = pcm;
    m_bufsize = bufsize;
    m_persize = persize;
    buf_in = new float[2*bufsize];
    memset(buf_in, 0, 2*bufsize*sizeof(float));
}

PCM_Capture::~PCM_Capture()
{
    sem_destroy(&mh_sem_thread_start);
    delete[] buf_in;
}

void PCM_Capture::set_transfer_interface(PCM_Transfer_Interface* xfer_iface)
{
    m_xfer_iface = xfer_iface;
}

void PCM_Capture::start()
{    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    sem_wait(&mh_sem_thread_start);
    cout << "Starting capture thread..." << endl;
    pthread_create(&mh_thread, &attr, PCM_Capture::class_thread_run_thunk, (void*)this);
    sem_wait(&mh_sem_thread_start);
    cout << "Capture thread started." << endl;
    pthread_attr_destroy(&attr);
}

void* PCM_Capture::stop()
{
    cout << "Stopping capture thread..." << endl;
    void* retval = 0;
    this->m_should_stop = true;
    pthread_join(mh_thread, &retval);
    cout << "Capture thread stopped." << endl;
        
    return retval;
}

void* PCM_Capture::class_thread_run_thunk(void* args)
{
    PCM_Capture* obj = reinterpret_cast<PCM_Capture*>(args);
    return obj->thread_run_func(args);
}

void* PCM_Capture::thread_run_func(void* args)
{
    this->m_should_stop = false;
    sem_post(&mh_sem_thread_start);
    
    try
    {
        this->prepare_capture_loop();
        
        while(false == this->m_should_stop)
        {
            this->run_capture_loop();
        }
    }
    catch(std::runtime_error& excp)
    {
        cout << "A runtime exception was thrown from the capture loop: " << excp.what() << endl;
    }
    
    snd_pcm_drop(mh_pcm);
    
    cout << "Capture thread func exit." << endl;
    return NULL;
}

void PCM_Capture::prepare_capture_loop()
{
    count = snd_pcm_poll_descriptors_count (mh_pcm);
    if(count <= 0) 
    {
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
    ptr = buf_in;
}

int PCM_Capture::wait_for_poll()
{
    unsigned short revents;
    while (1) 
    {
        poll(ufds, count, -1);
        snd_pcm_poll_descriptors_revents(mh_pcm, ufds, count, &revents);
        if (revents & POLLERR)
        {
            return -EIO;
        }
        if (revents & POLLIN)
        {
            return 0;
        }
    }
}

int PCM_Capture::xrun_recovery(int err)
{
        printf("Capture stream recovery\n");
        if (err == -EPIPE) {    /* under-run */
                err = snd_pcm_prepare(mh_pcm);
                if (err < 0)
                {
                    throw runtime_error(string("Can't recover from capture underrun, prepare failed: ") + snd_strerror(err));
                }
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
                        throw runtime_error(string("Can't recover from capture suspend, prepare failed: ") + snd_strerror(err));
                    }
                }
                return 0;
        }
        return err;
}

void PCM_Capture::run_capture_loop()
{
    if (!init) {
            err = wait_for_poll();
            if (err < 0) {
                    if (snd_pcm_state(mh_pcm) == SND_PCM_STATE_XRUN ||
                        snd_pcm_state(mh_pcm) == SND_PCM_STATE_SUSPENDED) {
                            err = snd_pcm_state(mh_pcm) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
                            if (xrun_recovery(err) < 0) {
                                    throw runtime_error(string("Read error: ") + snd_strerror(err));
                            }
                            init = 1;
                    } else {
                            throw runtime_error("Wait for poll failed");
                    }
            }
    }
    
    if(ptr == (buf_in + (m_bufsize*2)))
    {
        m_xfer_iface->write_buffer(buf_in, sizeof(float), m_bufsize*2);
        ptr = buf_in;
    }
    cptr = m_persize;
    while (cptr > 0) {
            err = snd_pcm_readi(mh_pcm, ptr, cptr);
            if (err < 0) 
            {
                if (xrun_recovery(err) < 0) 
                {
                    throw runtime_error(string("Read error: ") + snd_strerror(err));
                }
                init = 1;
                break;  /* skip one period */
            }
            if (snd_pcm_state(mh_pcm) == SND_PCM_STATE_RUNNING)
            {
                init = 0;
            }
            ptr += err * channels;
            cptr -= err;
            if (cptr == 0)
            {
                break;
            }
            /* it is possible that the initial buffer cannot store */
            /* all data from the last period, so wait awhile */
            err = wait_for_poll();
            if (err < 0) {
                    if (snd_pcm_state(mh_pcm) == SND_PCM_STATE_XRUN ||
                        snd_pcm_state(mh_pcm) == SND_PCM_STATE_SUSPENDED) {
                            err = snd_pcm_state(mh_pcm) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
                            if (xrun_recovery(err) < 0) {
                                    throw runtime_error(string("Read error: ") + snd_strerror(err));
                            }
                            init = 1;
                    } else {
                            throw runtime_error("Wait for poll failed");
                    }
            }
    }
}
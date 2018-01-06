#include "stream_capture.h"
#include <string.h>
#include <unistd.h>

using namespace std;

StreamCapture::StreamCapture(string filename, uint32_t framesize)
{
    m_sound_file = new SoundFile(filename);
    sem_init(&mh_sem_thread_start, 0, 1);
    m_framesize = framesize;
    buf_in = new float[2*framesize];
    memset(buf_in, 0, 2*framesize*sizeof(float));
    m_sleep_dur = (float)framesize/48000.0*1000000.0;
}

StreamCapture::~StreamCapture()
{
    sem_destroy(&mh_sem_thread_start);
    delete[] buf_in;
    delete m_sound_file;
}

void StreamCapture::set_transfer_interface(PCM_Transfer_Interface* xfer_iface)
{
    m_xfer_iface = xfer_iface;
}

void StreamCapture::start()
{    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    sem_wait(&mh_sem_thread_start);
    cout << "Starting stream capture thread..." << endl;
    pthread_create(&mh_thread, &attr, StreamCapture::class_thread_run_thunk, (void*)this);
    sem_wait(&mh_sem_thread_start);
    cout << "Stream capture thread started." << endl;
    pthread_attr_destroy(&attr);
}

void* StreamCapture::stop()
{
    cout << "Stopping stream capture thread..." << endl;
    void* retval = 0;
    this->m_should_stop = true;
    pthread_join(mh_thread, &retval);
    cout << "Stream capture thread stopped." << endl;
        
    return retval;
}

void* StreamCapture::class_thread_run_thunk(void* args)
{
    StreamCapture* obj = reinterpret_cast<StreamCapture*>(args);
    return obj->thread_run_func(args);
}

void* StreamCapture::thread_run_func(void* args)
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
        cout << "A runtime exception was thrown from the stream capture loop: " << excp.what() << endl;
    }
        
    cout << "Stream capture thread func exit." << endl;
    return NULL;
}

void StreamCapture::prepare_capture_loop()
{
    
}

void StreamCapture::run_capture_loop()
{
    while(false == this->m_should_stop)
    {
        m_sound_file->get_frames(m_framesize, buf_in);
        m_xfer_iface->write_buffer(buf_in, sizeof(float), m_framesize*2);
    }
}
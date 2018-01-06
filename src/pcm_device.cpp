#include "pcm_device.h"

#include <iostream>
#include <stdexcept>

#include "processor_simple_gain.h"

/* TODO: Need to open PCM capture to get buf params even though we might
 * be using a stream.  This is very hokey...need a way to abstract away
 * the capture parameters from the PCM device.
 */

PCM_Device::PCM_Device(string* name)
{
    m_name = new string(*name);
    m_stream_capture_enabled = false;
}

PCM_Device::~PCM_Device()
{
    this->stop();
    this->close();
    delete m_name;
}

void PCM_Device::open()
{
    int retval = snd_pcm_open(&mh_pb, m_name->c_str(), SND_PCM_STREAM_PLAYBACK, 0);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + "opening PCM playback device: " + snd_strerror(retval));
    }
    else
    {
        printf("PCM playback device opened.\n");
    }
    
    this->configure(this->mh_pb, string("playback"));
    m_pcm_pb = new PCM_Playback(this->mh_pb, m_bufsize_pb, m_persize_pb);
    
    retval = snd_pcm_open(&mh_cap, m_name->c_str(), SND_PCM_STREAM_CAPTURE, 0);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + "opening PCM capture device: " + snd_strerror(retval));
    }
    else
    {
        printf("PCM capture device opened.\n");
    }
    
    this->configure(this->mh_cap, string("capture"));
    m_pcm_cap = new PCM_Capture(this->mh_cap, m_bufsize_cap, m_persize_cap);
    
    if(true == m_stream_capture_enabled)
    {
        m_capture_stream = new StreamCapture(m_capture_stream_name, m_bufsize_cap);
    }
}

void PCM_Device::start()
{
    if(true == m_stream_capture_enabled)
    {
        m_capture_stream->start();
    }
    else
    {
        m_pcm_cap->start();
    }
    m_pcm_pb->start();
}

void PCM_Device::stop()
{
    if(true == m_stream_capture_enabled)
    {
        m_capture_stream->stop();
    }
    else
    {
        m_pcm_cap->stop();
    }
    m_pcm_pb->stop();
}

void PCM_Device::close()
{
    if(NULL != m_capture_stream)
    {
        cout << "Closing capture stream..." << endl;
        delete m_capture_stream;
        cout << "Capture stream closed." << endl;
    }
    
    cout << "Closing capture PCM..." << endl;
    snd_pcm_close(this->mh_cap);
    cout << "Capture PCM closed." << endl;

    cout << "Closing playback PCM..." << endl;
    snd_pcm_close(this->mh_pb);
    cout << "Playback PCM closed." << endl;
    
    
}

void PCM_Device::set_capture_mode_stream()
{
    m_stream_capture_enabled = true;
}

void PCM_Device::set_capture_stream(string stream_name)
{
    m_capture_stream_name = stream_name;
}

void PCM_Device::configure(snd_pcm_t* handle, string subdev_name)
{
    int retval = 0;
    snd_pcm_hw_params_t* hw_params = 0;
    snd_pcm_hw_params_malloc(&hw_params);
    
    retval = snd_pcm_hw_params_any(handle, hw_params);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + " getting PCM hw params for " + subdev_name + " device: " + snd_strerror(retval));
    }
    
    retval = snd_pcm_hw_params_set_access(handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + " setting PCM hw param 'access' for " + subdev_name + " device: " + snd_strerror(retval));
    }
    
    retval = snd_pcm_hw_params_set_format(handle, hw_params, SND_PCM_FORMAT_FLOAT_LE);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + " setting PCM hw param 'format' for " + subdev_name + " device: " + snd_strerror(retval));
    }
    
    retval = snd_pcm_hw_params_set_channels(handle, hw_params, 2);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + " setting PCM hw param 'channels' for " + subdev_name + " device: " + snd_strerror(retval));
    }
    
    retval = snd_pcm_hw_params_set_rate(handle, hw_params, 48000, 0);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + " setting PCM hw param 'rate' for " + subdev_name + " device: " + snd_strerror(retval));
    }
    
    retval = snd_pcm_hw_params_set_rate_resample(handle, hw_params, 0);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + " setting PCM hw param 'rate_resample' for " + subdev_name + " device: " + snd_strerror(retval));
    }
    
    snd_pcm_uframes_t buf_size_tmp = PCM_Device::BUFFER_SIZE_REQ;
    retval = snd_pcm_hw_params_set_buffer_size_near(handle, hw_params, &buf_size_tmp);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + " setting PCM hw param 'buffer_size' for " + subdev_name + " device: " + snd_strerror(retval));
    }
    
    if(0 == subdev_name.compare("playback"))
    {
        m_bufsize_pb = buf_size_tmp;
    }
    else
    {
        m_bufsize_cap = buf_size_tmp;
    }
    
    snd_pcm_uframes_t per_size_tmp = PCM_Device::PERIOD_SIZE_REQ;
    retval = snd_pcm_hw_params_set_period_size_near(handle, hw_params, &per_size_tmp, 0);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + " setting PCM hw param 'period_size' for " + subdev_name + " device: " + snd_strerror(retval));
    }
    
    if(0 == subdev_name.compare("playback"))
    {
        m_persize_pb = per_size_tmp;
    }
    else
    {
        m_persize_cap = per_size_tmp;
    }
    
    snd_pcm_sw_params_t* sw_params = 0;
    snd_pcm_sw_params_malloc(&sw_params);
    snd_pcm_sw_params_current(handle, sw_params);
    snd_pcm_sw_params_free(sw_params);
        
    retval = snd_pcm_hw_params(handle, hw_params);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + " setting PCM hw params for " + subdev_name + " device: " + snd_strerror(retval));
    }
    
    cout << "Finished setting HW params for " + subdev_name + " device." << endl;
    
    
    if(0 == subdev_name.compare("playback"))
    {
        cout << "PCM " + subdev_name + " HW buffer size is " + to_string(m_bufsize_pb) + " frames." << endl;
        cout << "PCM " + subdev_name + " HW period size is " + to_string(m_persize_pb) + " frames." << endl;
    }
    else
    {
        cout << "PCM " + subdev_name + " HW buffer size is " + to_string(m_bufsize_cap) + " frames." << endl;
        cout << "PCM " + subdev_name + " HW period size is " + to_string(m_persize_cap) + " frames." << endl;
    }
    
    snd_pcm_hw_params_free(hw_params);
}

void PCM_Device::set_audio_hub(AudioHub* audio_hub)
{
    m_audio_hub = audio_hub;
    m_audio_hub->configure(m_bufsize_cap, m_persize_cap, m_bufsize_pb, m_persize_pb);
    m_pcm_pb->set_transfer_interface(m_audio_hub);
    if(true == m_stream_capture_enabled)
    {
        m_capture_stream->set_transfer_interface(m_audio_hub);
    }
    else
    {
        m_pcm_cap->set_transfer_interface(m_audio_hub);
    }
}
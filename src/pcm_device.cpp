#include "pcm_device.h"

#include <iostream>
#include <stdexcept>

PCM_Device::PCM_Device(string* name)
{
    m_name = new string(*name);
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
    
    
    
}

void PCM_Device::start()
{
    m_pcm_pb->start();
}

void PCM_Device::stop()
{
    m_pcm_pb->stop();
}

void PCM_Device::close()
{
    cout << "Closing PCM..." << endl;
    snd_pcm_close(this->mh_cap);
    snd_pcm_close(this->mh_pb);
    cout << "PCM closed." << endl;
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
    
    retval = snd_pcm_hw_params_set_format(handle, hw_params, SND_PCM_FORMAT_S16_LE);
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
    
    snd_pcm_uframes_t per_size_tmp = PCM_Device::PERIOD_SIZE_REQ;
    retval = snd_pcm_hw_params_set_period_size_near(handle, hw_params, &per_size_tmp, 0);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + " setting PCM hw param 'period_size' for " + subdev_name + " device: " + snd_strerror(retval));
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
    
    snd_pcm_uframes_t bufsize = 0;
    retval = snd_pcm_hw_params_get_buffer_size(hw_params, &bufsize);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + " getting PCM hw params 'buffer_size' for " + subdev_name + " device: " + snd_strerror(retval));
    }
    cout << "PCM " + subdev_name + " HW buffer size is " + to_string(bufsize) + " frames." << endl;
    
    unsigned int buftime = 0;
    retval = snd_pcm_hw_params_get_buffer_time(hw_params, &buftime, 0);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + " getting PCM hw params 'buffer_time' for " + subdev_name + " device: " + snd_strerror(retval));
    }
    
    cout << "PCM " + subdev_name + " HW buffer time is " + to_string(buftime) + " us." << endl;
    
    snd_pcm_uframes_t persize = 0;
    retval = snd_pcm_hw_params_get_period_size(hw_params, &persize, 0);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + " getting PCM hw params 'period_size' for " + subdev_name + " device: " + snd_strerror(retval));
    }
    cout << "PCM " + subdev_name + " HW period size is " + to_string(persize) + " frames." << endl;
    
    unsigned int pertime = 0;
    retval = snd_pcm_hw_params_get_period_time(hw_params, &pertime, 0);
    if(0 > retval)
    {
        throw runtime_error("Error " + to_string(retval) + " getting PCM hw params 'period_time' for " + subdev_name + " device: " + snd_strerror(retval));
    }
    
    cout << "PCM " + subdev_name + " HW period time is " + to_string(pertime) + "us." << endl;
    
    snd_pcm_hw_params_free(hw_params);
    
    m_pcm_pb = new PCM_Playback(this->mh_pb, bufsize, persize);
}
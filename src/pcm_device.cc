#include "pcm_device.h"
#include <stdexcept>
#include <sstream>
#include <stdio.h>

PCMDevice::PCMDevice(string& _name, string& _desc, int _ioid, int _mode)
{
	dev = NULL;
	ioid = -1;
	name = new string(_name);
	desc = new string(_desc);

	switch(_ioid)
	{
	case PCMDevice::IOID_OUTPUT:
	case PCMDevice::IOID_INPUT:
	case PCMDevice::IOID_BOTH:
		ioid = _ioid;
		break;
	default:
		throw runtime_error("Unknown IOID.");
	}

	switch(_mode)
	{
	case 0:
		mode = "PLAYBACK";
		break;
	case 1:
		mode = "CAPTURE";
		break;
	default:
		mode = "UNKNOWN";
	}

	period_event = 0;
}

PCMDevice::~PCMDevice()
{
	delete name;
	delete desc;
}

/*
 *   Underrun and suspend recovery
 */

int PCMDevice::xrun_recovery(int err)
{
    cout << "stream recovery" << endl;

    if (err == -EPIPE) /* under-run */
    {
        err = snd_pcm_prepare(dev);
        if (err < 0)
        {
        	cout << "Can't recovery from underrun, prepare failed: %s\n" << snd_strerror(err) << endl;
        }
        return 0;
    }
    else if (err == -ESTRPIPE)
    {
        while ((err = snd_pcm_resume(dev)) == -EAGAIN)
        {
            sleep(1);   /* wait until the suspend flag is released */
        }
        if (err < 0)
        {
            err = snd_pcm_prepare(dev);
            if (err < 0)
            {
            	cout << "Can't recovery from suspend, prepare failed: %s\n" << snd_strerror(err) << endl;
            }
        }
        return 0;
    }
    return err;
}

void PCMDevice::dump_info()
{
	if(dev)
	{
		snd_output_t* sop;
		snd_output_stdio_attach(&sop, stdout, 0);
		snd_pcm_dump(dev, sop);
	}
}

void PCMDevice::set_hw_params(snd_pcm_access_t access,
							 int resample, snd_pcm_format_t format, int channels,
							 unsigned int rate, unsigned int buffer_time, unsigned int period_time)
{
	snd_pcm_hw_params_alloca(&hw_params);

    unsigned int rrate;
    snd_pcm_uframes_t size;
    int err, dir;
    /* choose all parameters */
    err = snd_pcm_hw_params_any(dev, hw_params);
    if (err < 0) {
        printf("Broken configuration for %s: no configurations available: %s\n", mode.c_str(), snd_strerror(err));
    }
    /* set hardware resampling */
    err = snd_pcm_hw_params_set_rate_resample(dev, hw_params, resample);
    if (err < 0) {
        printf("Resampling setup failed for %s: %s\n", mode.c_str(), snd_strerror(err));
    }
    /* set the interleaved read/write format */
    err = snd_pcm_hw_params_set_access(dev, hw_params, access);
    if (err < 0) {
        printf("Access type not available for %s: %s\n", mode.c_str(), snd_strerror(err));
    }
    /* set the sample format */
    err = snd_pcm_hw_params_set_format(dev, hw_params, format);
    if (err < 0) {
        printf("Sample format not available for %s: %s\n", mode.c_str(), snd_strerror(err));
    }
    /* set the count of channels */
    err = snd_pcm_hw_params_set_channels(dev, hw_params, channels);
    if (err < 0) {
        printf("Channel count (%u) not available for %s: %s\n", channels, mode.c_str(), snd_strerror(err));
    }
    /* set the stream rate */
    rrate = rate;
    err = snd_pcm_hw_params_set_rate_near(dev, hw_params, &rrate, 0);
    if (err < 0) {
        printf("Rate %uHz not available for %s: %s\n", rate, mode.c_str(), snd_strerror(err));
    }
    if (rrate != rate) {
        printf("Rate doesn't match (requested %uHz, get %iHz)\n", rate, err);
    }
    /* set the buffer time */
    err = snd_pcm_hw_params_set_buffer_time_near(dev, hw_params, &buffer_time, &dir);
    if (err < 0) {
        printf("Unable to set buffer time %u for %s: %s\n", buffer_time, mode.c_str(), snd_strerror(err));
    }
    err = snd_pcm_hw_params_get_buffer_size(hw_params, &size);
    if (err < 0) {
        printf("Unable to get buffer size for %s: %s\n", mode.c_str(), snd_strerror(err));
    }
    buffer_size = size;
    /* set the period time */
    err = snd_pcm_hw_params_set_period_time_near(dev, hw_params, &period_time, &dir);
    if (err < 0) {
        printf("Unable to set period time %u for %s: %s\n", period_time, mode.c_str(), snd_strerror(err));
    }
    err = snd_pcm_hw_params_get_period_size(hw_params, &size, &dir);
    if (err < 0) {
        printf("Unable to get period size for %s: %s\n", mode.c_str(), snd_strerror(err));
    }
    period_size = size;
    /* write the parameters to device */
    err = snd_pcm_hw_params(dev, hw_params);
    if (err < 0) {
        printf("Unable to set hw params for %s: %s\n", mode.c_str(), snd_strerror(err));
    }

    cout << *(this->name) << " buffer size: " << buffer_size << ", period size: " << period_size << endl;
}

void PCMDevice::set_sw_params()
{
	int err;
	snd_pcm_sw_params_alloca(&sw_params);

	/* get the current swparams */
	err = snd_pcm_sw_params_current(dev, sw_params);
	if (err < 0) {
		printf("Unable to determine current swparams for %s: %s\n", mode.c_str(), snd_strerror(err));
	}
	/* start the transfer when the buffer is almost full: */
	/* (buffer_size / avail_min) * avail_min */
	err = snd_pcm_sw_params_set_start_threshold(dev, sw_params, (buffer_size / period_size) * period_size);
	if (err < 0) {
		printf("Unable to set start threshold mode for %s: %s\n", mode.c_str(), snd_strerror(err));
	}
	/* allow the transfer when at least period_size samples can be processed */
	/* or disable this mechanism when period event is enabled (aka interrupt like style processing) */
	err = snd_pcm_sw_params_set_avail_min(dev, sw_params, period_event ? buffer_size : period_size);
	if (err < 0) {
		printf("Unable to set avail min for %s: %s\n", mode.c_str(), snd_strerror(err));
	}
	/* enable period events when requested */
	if (period_event) {
		err = snd_pcm_sw_params_set_period_event(dev, sw_params, 1);
		if (err < 0) {
			printf("Unable to set period event for %s: %s\n", mode.c_str(), snd_strerror(err));
		}
	}
	/* write the parameters to the playback device */
	err = snd_pcm_sw_params(dev, sw_params);
	if (err < 0) {
		printf("Unable to set sw params for %s: %s\n", mode.c_str(), snd_strerror(err));
	}
}

int PCMDevice::wait_for_poll()
{
	unsigned short revents;

	while (1) {
		poll(pfd, pfd_count, -1);

		snd_pcm_poll_descriptors_revents(dev, pfd, pfd_count, &revents);

		if (revents & POLLERR)
		{
			return -EIO;
		}
		if (revents & POLLOUT)
		{
			return POLLOUT;
		}
		if (revents & POLLIN)
		{
			return POLLIN;
		}
	}

	return 0;
}

void PCMDevice::close()
{
	int retval = -1;
	retval = snd_pcm_close(dev);

	if(retval)
	{
		stringstream ss;
		ss << "Error closing PCM device " << this->name << ": " << snd_strerror(retval);
		throw runtime_error(ss.str());
	}

}

string PCMDevice::get_name()
{
	return string(*name);
}

string PCMDevice::get_desc()
{
	return string(*desc);
}

int PCMDevice::get_ioid()
{
	return ioid;
}

uint32_t PCMDevice::get_buffer_size()
{
	return buffer_size;
}

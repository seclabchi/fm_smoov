#include "pcm_device_capture.h"
#include <stdexcept>
#include <sstream>

//#define DEBUG_CAPTURE

PCMDeviceCapture::PCMDeviceCapture(string& _name, string& _desc, int _ioid) : PCMDevice(_name, _desc, _ioid, 1)
{
	ringbuf = AudioBuffer::get_instance();
}

PCMDeviceCapture::~PCMDeviceCapture()
{
	delete name;
	delete desc;

}

void PCMDeviceCapture::open()
{
	int retval = -1;
	retval = snd_pcm_open(&dev, this->name->c_str(), SND_PCM_STREAM_CAPTURE, 0);

	if(retval)
	{
		stringstream ss;
		ss << "Error opening PCM device " << this->name << ": " << snd_strerror(retval);
		throw runtime_error(ss.str());
	}

	pfd_count = snd_pcm_poll_descriptors_count (dev);
	pfd = (struct pollfd*)malloc(sizeof(struct pollfd) * pfd_count);
	//cout << "pfd = " << pfd << endl;
	//cout << pfd_count << " = pfd_count" << endl;
	snd_pcm_poll_descriptors(dev, pfd, pfd_count);

	//800000, 200000 best so far
	this->set_hw_params(SND_PCM_ACCESS_RW_INTERLEAVED,
				 	 	 	 0, SND_PCM_FORMAT_FLOAT_LE, 2,
							 48000, 800000, 200000);

	this->set_sw_params();
}

void PCMDeviceCapture::start()
{
	//cout << "Dumping CAPTURE device info: " << endl;
	//this->dump_info();
	snd_pcm_start(dev);
}

void PCMDeviceCapture::stop()
{
	snd_pcm_drop(dev);
}

uint32_t PCMDeviceCapture::capture_frames()
{
	snd_pcm_sframes_t devret;

	int err = wait_for_poll();

	if( POLLIN == err )
	{
		snd_pcm_sframes_t avail = snd_pcm_avail(dev);

		//cout << "capture avail: " << avail << endl;

		if(avail > 0)
		{
			devret = snd_pcm_readi(dev, buf, avail);

			if(devret < 0)
			{
				cout << "Capture error: " << snd_strerror(devret) << endl;
				this->xrun_recovery(devret);
			}
			else
			{
				//cout << "Captured " << devret << " frames." << endl;
				ringbuf->write_frames(buf, avail);
			}
		}
		else
		{
			cout << "capture avail error: " << snd_strerror(avail) << endl;
			this->xrun_recovery(avail);
		}
	}
	else
	{
		cout << "CAPTURE POLL: " << snd_strerror(err) << endl;
		snd_pcm_recover(dev, 0, 1);
	}

	return devret;

}


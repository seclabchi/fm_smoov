#include "pcm_device_playback.h"
#include <stdexcept>
#include <sstream>

PCMDevicePlayback::PCMDevicePlayback(string& _name, string& _desc, int _ioid) : PCMDevice(_name, _desc, _ioid, 0)
{
	ringbuf = AudioBuffer::get_instance();
	sine_gen = new SineWaveGen(48000, 440, true, 24);
}

PCMDevicePlayback::~PCMDevicePlayback()
{
	delete name;
	delete desc;

}

void PCMDevicePlayback::open()
{
	int retval = -1;
	retval = snd_pcm_open(&dev, this->name->c_str(), SND_PCM_STREAM_PLAYBACK, 0);

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

	//500000, 125000 best so far
	this->set_hw_params(SND_PCM_ACCESS_RW_INTERLEAVED,
				 	 	 	 0, SND_PCM_FORMAT_FLOAT_LE, 2,
							 48000, 500000, 125000);

	this->set_sw_params();
}

void PCMDevicePlayback::start()
{


	//cout << "Dumping PLAYBACK device info: " << endl;
	//this->dump_info();
	snd_pcm_start(dev);
}

void PCMDevicePlayback::stop()
{
	snd_pcm_drop(dev);
}

uint32_t PCMDevicePlayback::playback_frames()
{
	if(POLLOUT == wait_for_poll())
	{
		snd_pcm_sframes_t devret = 0;

		snd_pcm_sframes_t dev_avail = 0;
		snd_pcm_sframes_t buf_avail = 0;

		dev_avail = snd_pcm_avail(dev);

		//cout << "PB DEV AVAIL " << dev_avail << endl;

		if(dev_avail > 0)
		{
			buf_avail = ringbuf->read_frames(buf, dev_avail);

			/*for(uint32_t i = 0; i < dev_avail * 2; i+=2)
			{
				buf[i] = sine_gen->get_next_value();
				buf[i+1] = buf[i];
			}

			buf_avail = dev_avail;
			*/

			if(devret < 0)
			{
				cout << "Playback error: " << snd_strerror(devret) << endl;
				this->xrun_recovery(devret);
			}

			devret = snd_pcm_writei(dev, buf, dev_avail);
			//cout << "PB dev_avail=" << dev_avail << ", wrote " << devret << endl;

			if(devret < 0)
			{
				cout << "Playback error: " << snd_strerror(devret) << endl;
				this->xrun_recovery(devret);
			}
			else
			{
				//cout << buf_avail - devret << " samples left over on PB" << endl;
			}
		}
	}
}


#ifndef __PCM_DEVICE_H__
#define __PCM_DEVICE_H__

#include <alsa/asoundlib.h>
#include <string>
#include <iostream>
#include <semaphore.h>

using namespace std;

class PCMDevice
{
public:
	PCMDevice(string& name, string& desc, int ioid, int mode);
	virtual ~PCMDevice();
	static const int IOID_OUTPUT = 0;
	static const int IOID_INPUT = 1;
	static const int IOID_BOTH = 2;

	string get_name();
	string get_desc();
	int get_ioid();

	virtual void open() = 0;
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void close();

	void dump_info();

	virtual uint32_t capture_frames() = 0;
	virtual uint32_t playback_frames() = 0;

	snd_pcm_uframes_t get_buffer_size_frames();
	snd_pcm_uframes_t get_period_size_frames();

	uint32_t get_buffer_size();

protected:
	snd_pcm_t* dev;

	float buf[200000];

	int xrun_recovery(int err);
	int wait_for_poll();
	struct pollfd* pfd;
	int pfd_count;

	void set_hw_params(snd_pcm_access_t access,
			 	 	 	 int resample, snd_pcm_format_t format, int channels,
						 unsigned int rate, unsigned int buffer_time, unsigned int period_time);
	void set_sw_params();
	snd_pcm_hw_params_t* hw_params;
	snd_pcm_sframes_t buffer_size;
	snd_pcm_sframes_t period_size;

	snd_pcm_sw_params_t* sw_params;
	int period_event;

	string* name;
	string* desc;
	int ioid;

	string mode;
};

#endif

#ifndef __PCM_DEVICE_PLAYBACK_H__
#define __PCM_DEVICE_PLAYBACK_H__

#include <alsa/asoundlib.h>
#include <string>
#include "pcm_device.h"
#include "audio_buffer.h"

#include "sine_wave_gen.h"

using namespace std;

class PCMDevicePlayback : public PCMDevice
{
public:
	PCMDevicePlayback(string& name, string& desc, int ioid);
	virtual ~PCMDevicePlayback();

	virtual void open();
	virtual void start();
	virtual void stop();

	inline virtual uint32_t capture_frames() {return 0;};
	virtual uint32_t playback_frames();

private:
	AudioBuffer* ringbuf;
	SineWaveGen* sine_gen;
	float tmpbuf[100000];
};

#endif

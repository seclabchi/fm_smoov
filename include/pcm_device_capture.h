#ifndef __PCM_DEVICE_CAPTURE_H__
#define __PCM_DEVICE_CAPTURE_H__

#include <alsa/asoundlib.h>
#include <string>
#include "pcm_device.h"
#include "audio_buffer.h"

using namespace std;

class PCMDeviceCapture : public PCMDevice
{
public:
	PCMDeviceCapture(string& name, string& desc, int ioid);
	virtual ~PCMDeviceCapture();

	virtual void open();
	virtual void start();
	virtual void stop();

	virtual uint32_t capture_frames();
	inline virtual uint32_t playback_frames() {return 0;};

private:
	FILE* debug_out;
	AudioBuffer* ringbuf;
};

#endif

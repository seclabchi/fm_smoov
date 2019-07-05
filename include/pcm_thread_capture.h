#ifndef __PCM_THREAD_CAPTURE_H__
#define __PCM_THREAD_CAPTURE_H__

#include "pcm_thread.h"
#include "pcm_device_capture.h"

class PCMThreadCapture : public PCMThread
{
public:
	PCMThreadCapture(PCMDevice* dev, uint32_t _pb_bufsize);
	virtual ~PCMThreadCapture();

protected:
	void* thread_func_internal();
	uint32_t pb_bufsize;
};

#endif

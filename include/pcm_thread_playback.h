#ifndef __PCM_THREAD_PLAYBACK_H__
#define __PCM_THREAD_PLAYBACK_H__

#include "pcm_thread.h"
#include "pcm_device_playback.h"

class PCMThreadPlayback : public PCMThread
{
public:
	PCMThreadPlayback(PCMDevice* dev);
	virtual ~PCMThreadPlayback();

protected:
	void* thread_func_internal();
};

#endif

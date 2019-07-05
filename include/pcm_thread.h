#ifndef __PCM_THREAD_H__
#define __PCM_THREAD_H__

#include <pthread.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <semaphore.h>

#include "pcm_device.h"

using namespace std;

class PCMThread
{
	public:
	PCMThread(int _mode, PCMDevice* _dev);
	virtual ~PCMThread();

	void start(sem_t* _sem_cap_start, sem_t* _sem_pb_opened);
	void stop();

	static const int CAPTURE_MODE = 0;
	static const int PLAYBACK_MODE = 1;

	static void* thread_func(void* args);

	protected:
	virtual void* thread_func_internal() = 0;
	string* device_name;
	string* thread_name;
	bool shutdown;
	PCMDevice* dev;
	sem_t* sem_cap_start;
	sem_t* sem_pb_opened;

	private:
	pthread_t the_thread;
	int mode;


};

#endif

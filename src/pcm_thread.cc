#include "pcm_thread.h"

#include <unistd.h>
#include <string.h>
#include <sstream>

PCMThread::PCMThread(int _mode, PCMDevice* _dev)
{
	shutdown = false;

	stringstream name_str;

	the_thread = 0;

	if((PCMThread::CAPTURE_MODE != _mode) && (PCMThread::PLAYBACK_MODE != _mode))
	{
		throw runtime_error("Unknown PCMThread mode.");
	}

	mode = _mode;
	dev = _dev;
	device_name = new string(dev->get_name());

	name_str << *device_name;

	if(PCMThread::CAPTURE_MODE == mode)
	{
		name_str << "_CAPTURE";
	}
	else
	{
		name_str << "_PLAYBACK";
	}

	thread_name = new string(name_str.str());
}

PCMThread::~PCMThread()
{
	delete device_name;
	delete thread_name;
}

void PCMThread::start(sem_t* _sem_cap_start, sem_t* _sem_pb_opened)
{
	sem_cap_start = _sem_cap_start;
	sem_pb_opened = _sem_pb_opened;

	pthread_attr_t attr;
	int retval = 0;

	retval = pthread_attr_init(&attr);
	if(retval)
	{
		throw runtime_error(strerror(retval));
	}

	retval = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	if(retval)
	{
		throw runtime_error(strerror(retval));
	}

	retval = pthread_create(&the_thread, &attr, PCMThread::thread_func, this);
	if(retval)
	{
		throw runtime_error(strerror(retval));
	}

	retval = pthread_attr_destroy(&attr);
	if(retval)
	{
		throw runtime_error(strerror(retval));
	}
}

void PCMThread::stop()
{
	shutdown = true;
	void* thread_return;

	int retval = pthread_join(the_thread, &thread_return);

	if(0 != retval)
	{
		throw runtime_error(strerror(retval));
	}
}

void* PCMThread::thread_func(void* args)
{
	PCMThread* me = (PCMThread*)args;
	return me->thread_func_internal();
}



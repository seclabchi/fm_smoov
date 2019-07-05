#include "pcm_thread_playback.h"
#include <unistd.h>

PCMThreadPlayback::PCMThreadPlayback(PCMDevice* dev) : PCMThread(PCMThread::PLAYBACK_MODE, dev)
{

}

PCMThreadPlayback::~PCMThreadPlayback()
{

}

void* PCMThreadPlayback::thread_func_internal()
{
	cout << "Entering PCMThread " << *thread_name << endl;

	try
	{
		dev->open();
		sem_post(sem_pb_opened);
		cout << "Waitng for cap to start..." << endl;
		sem_wait(sem_cap_start);
		dev->start();

		do
		{
			dev->playback_frames();
		} while(false == shutdown);

		cout << "Exiting PCMThread " << *thread_name << endl;
		dev->stop();
		dev->close();
	}
	catch(const exception& ex)
	{
		cout << "Exception in PCM playback thread: " << ex.what() << endl;
	}

	return NULL;
}

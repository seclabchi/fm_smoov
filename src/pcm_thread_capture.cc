#include "pcm_thread_capture.h"
#include <unistd.h>

PCMThreadCapture::PCMThreadCapture(PCMDevice* dev, uint32_t _pb_bufsize) : PCMThread(PCMThread::CAPTURE_MODE, dev)
{
	pb_bufsize = _pb_bufsize;
}

PCMThreadCapture::~PCMThreadCapture()
{

}

void* PCMThreadCapture::thread_func_internal()
{
	cout << "Entering PCMThread " << *thread_name << endl;

	try
	{
		uint8_t loopcnt = 0;
		dev->open();
		dev->start();
		bool fired = false;
		uint32_t frames_captured = 0;

		do
		{
			frames_captured += dev->capture_frames();
			//cout << "frames_captured = " << frames_captured << endl;
			if(frames_captured >= pb_bufsize * 2)
			{
				sem_post(sem_cap_start);
			}
		} while(false == shutdown);

		cout << "Exiting PCMThread " << *thread_name << endl;
		dev->stop();
		dev->close();
	}
	catch(const exception& ex)
	{
		cout << "Exception in PCM capture thread: " << ex.what() << endl;
	}

	return NULL;
}

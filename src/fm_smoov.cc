#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <semaphore.h>

#include "pcm_devices.h"
#include "pcm_thread_capture.h"
#include "pcm_thread_playback.h"
#include "pcm_device_capture.h"
#include "pcm_device_playback.h"

using namespace std;

int main (int argc, char *argv[])
{
	try
	{
		PCMDevices* devs = PCMDevices::getInstance();

		vector<PCMDevice*> playback_devices = devs->get_playback_devices();
		vector<PCMDevice*> capture_devices = devs->get_capture_devices();

		cout << "# playback devices: " << playback_devices.size() << endl;
		cout << "# capture devices: " << capture_devices.size() << endl;

		PCMDevice* dev_pb = playback_devices.at(1);
		PCMDevice* dev_cap = capture_devices.at(1);

		//start the playback thread.  It will signal when it's opened and ready for playback
		//then start the capture thread with the PB thread's buffer size, so the PB buffer can
		//be instantly filled to prevent an underrun.

		sem_t sem_cap_start;
		sem_init(&sem_cap_start, 0, 0);

		sem_t sem_pb_opened;
		sem_init(&sem_pb_opened, 0, 0);

		cout << "Playback device: " << playback_devices.at(1)->get_name() << endl;
		PCMThreadPlayback* playback_thread = new PCMThreadPlayback(dev_pb);
		playback_thread->start(&sem_cap_start, &sem_pb_opened);
		cout << "Waiting for PB to be opened..." << endl;
		sem_wait(&sem_pb_opened);
		uint32_t pb_buf_size = dev_pb->get_buffer_size();

		cout << "Capture device: " << capture_devices.at(1)->get_name() << endl;
		PCMThreadCapture* capture_thread = new PCMThreadCapture(dev_cap, pb_buf_size);
		capture_thread->start(&sem_cap_start, &sem_pb_opened);

		getchar();
		playback_thread->stop();
		capture_thread->stop();
	}
	catch(const exception& ex)
	{
		cout << "Failed in main: " << ex.what() << endl;
	}

	return 0;
}

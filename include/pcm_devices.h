#ifndef __PCM_DEVICES_H__
#define __PCM_DEVICES_H__

#include <iostream>
#include <vector>
#include "pcm_device.h"

using namespace std;

class PCMDevices
{
public:
	static PCMDevices* getInstance();
	~PCMDevices();
	vector<PCMDevice*> get_playback_devices();
	vector<PCMDevice*> get_capture_devices();

private:
	PCMDevices();
	void enumerate();

	static PCMDevices* theInstance;
	vector<PCMDevice*>* cap_devs;
	vector<PCMDevice*>* pb_devs;
};

#endif

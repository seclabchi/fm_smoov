#include "pcm_devices.h"

#include <stdexcept>
#include <alsa/asoundlib.h>
#include "pcm_device_capture.h"
#include "pcm_device_playback.h"

PCMDevices* PCMDevices::theInstance = NULL;

PCMDevices* PCMDevices::getInstance()
{
	if(NULL == theInstance)
	{
		theInstance = new PCMDevices();
	}

	return theInstance;
}

PCMDevices::PCMDevices()
{
	cap_devs = new vector<PCMDevice*>();
	pb_devs = new vector<PCMDevice*>();
	enumerate();
}

PCMDevices::~PCMDevices()
{
	vector<PCMDevice*>::iterator it;

	for(it = cap_devs->begin(); it != cap_devs->end(); it++)
	{
		delete *it;
	}

	for(it = pb_devs->begin(); it != pb_devs->end(); it++)
	{
		delete *it;
	}

	delete cap_devs;
	delete pb_devs;
}

vector<PCMDevice*> PCMDevices::get_playback_devices()
{
	vector<PCMDevice*> devs;
	vector<PCMDevice*>::iterator it;

	for(it = pb_devs->begin(); it != pb_devs->end(); it++)
	{
		devs.push_back((*it));
	}

	return devs;
}

vector<PCMDevice*> PCMDevices::get_capture_devices()
{
	vector<PCMDevice*> devs;

	vector<PCMDevice*>::iterator it;

	for(it = cap_devs->begin(); it != cap_devs->end(); it++)
	{
		devs.push_back((*it));
	}

	return devs;
}

void PCMDevices::enumerate()
{
	void** hints;
	int retval = 0;

	retval = snd_device_name_hint(-1, "pcm", &hints);

	if(0 != retval)
	{
		throw runtime_error(snd_strerror(retval));
	}

	void* hint = NULL;
	int hint_num = 0;

	int cap_dev_num = 0;
	int pb_dev_num = 0;

	do
	{
		hint = hints[hint_num];

		if(NULL != hint)
		{
			char* name = snd_device_name_get_hint(hint, "NAME");
			string namestr = string(name);
			char* desc = snd_device_name_get_hint(hint, "DESC");
			string descstr = string(desc);
			char* ioid = snd_device_name_get_hint(hint, "IOID");
			int ioid_int = -1;

			if(NULL != ioid)
			{
				 if(!strcmp(ioid, "Output"))
				 {
					 ioid_int = PCMDevice::IOID_OUTPUT;
					 pb_devs->push_back(new PCMDevicePlayback(namestr, descstr, ioid_int));

					 cout << "PLAYBACK " << pb_dev_num << " " << name << endl;
					 pb_dev_num++;

				 }
				 else
				 {
					 ioid_int = PCMDevice::IOID_INPUT;
					 cap_devs->push_back(new PCMDeviceCapture(namestr, descstr, ioid_int));
					 cout << "CAPTURE " << cap_dev_num << " " << name << endl;
					 cap_dev_num++;
				 }
			}
			else
			{
				ioid_int = PCMDevice::IOID_BOTH;
				cap_devs->push_back(new PCMDeviceCapture(namestr, descstr, ioid_int));
				cout << "CAPTURE " << cap_dev_num << " " << name << endl;
				cap_dev_num++;
				pb_devs->push_back(new PCMDevicePlayback(namestr, descstr, ioid_int));
				cout << "PLAYBACK " << pb_dev_num << " " << name << endl;
				pb_dev_num++;
			}

			hint_num++;
		}
	} while(NULL != hint);

	snd_device_name_free_hint(hints);
}

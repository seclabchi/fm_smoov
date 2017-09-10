#include <stdio.h>

#include <string>

#include <asoundlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "device_db.h"
#include "pcm_device.h"
#include "pcm_playback.h"

#include "processor_simple_gain.h"
#include "processor_slow_agc.h"

using namespace std;

int main(int argc, char **argv)
{
    DeviceDb* dev_db = new DeviceDb();
    size_t num_devs = dev_db->find_bi_devices();
    cout << "Found " << num_devs << " bi device(s)." << endl;
    string* dev_str = 0;
    dev_db->get_device_name(0, &dev_str);
    cout << "PCM device to be used: " << *dev_str << endl;
    
    AudioHub* audio_hub = new AudioHub();
    PCM_Device* dev = new PCM_Device(dev_str);
    dev->open();
    dev->set_audio_hub(audio_hub);
    
    ProcessorSimpleGain* psg = new ProcessorSimpleGain();
    audio_hub->add_processor(psg);
    ProcessorSlowAGC* pslowagc = new ProcessorSlowAGC();
    audio_hub->add_processor(pslowagc);
    
    dev->start();
    getchar();
    dev->stop();
    
    dev->close();
    
    delete dev_db;
    printf("Goodbye.\n");
	return 0;
}

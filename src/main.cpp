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
#include "processor_analyzer.h"
#include "tone_generator.h"

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
    
    ProcessorSimpleGain* psg = new ProcessorSimpleGain(48000);
    audio_hub->add_processor(psg);
    
    ProcessorAnalyzer* pslowagc = new ProcessorAnalyzer(48000);
    audio_hub->add_processor(pslowagc);
    
    ToneGenerator* tg = new ToneGenerator(48000);
    audio_hub->add_processor(tg);
    
    dev->start();
    
    getchar();
    dev->stop();
    
    dev->close();
    
    delete dev_db;
    printf("Goodbye.\n");
	return 0;
}

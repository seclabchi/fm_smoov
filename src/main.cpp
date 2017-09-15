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
    psg->set_gain(0.0, 0.0);
    
    dev->start();
    
    /* SIMPLE GAIN TESTS */
    sleep(5);
    psg->set_gain(-3.0, -3.0);
    sleep(5);
    psg->set_gain(-6.0, -6.0);
    sleep(5);
    psg->set_gain(-9.0, -9.0);
    sleep(5);
    psg->set_gain(-12.0, -12.0);
    sleep(5);
    psg->set_gain(-15.0, -15.0);
    sleep(5);
    psg->set_gain(-18.0, -18.0);
    sleep(5);
    psg->set_gain(-0.0, -0.0);
    
    /*sleep(2);
    
    ToneGenerator* tg = new ToneGenerator(48000);
    audio_hub->add_processor(tg);
    tg->enable_channels(true, true);
    
    sleep(2);
    tg->enable_channels(true, false);
    tg->set_frequency(500.0);
    sleep(2);
    tg->enable_channels(false, true);
    tg->set_frequency(2000.0);
    sleep(2);
    tg->enable_channels(true, false);
    tg->set_frequency(8000.0);
    sleep(2);
    tg->enable_channels(true, true);
    tg->set_frequency(440.0);
    sleep(2);
    tg->enable_channels(false, false);*/
    getchar();
    dev->stop();
    
    dev->close();
    
    delete dev_db;
    printf("Goodbye.\n");
	return 0;
}

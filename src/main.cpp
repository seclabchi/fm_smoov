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
#include "processor_slow_agc.h"
#include "processor_lpf.h"
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
    psg->set_gain(-12.0, -12.0);
    
    ProcessorAnalyzer* pa1 = new ProcessorAnalyzer(48000);
    audio_hub->add_processor(pa1);
    
    ProcessorLPF* plpf = new ProcessorLPF(48000);
    audio_hub->add_processor(plpf);
    //ProcessorSlowAgc* psagc = new ProcessorSlowAgc(48000);
    //psagc->set_attack_release(2.0, 2.0);
    //psagc->set_target_lin(30000.0);
    //audio_hub->add_processor(psagc);
    
    ProcessorAnalyzer* pa2 = new ProcessorAnalyzer(48000);
    audio_hub->add_processor(pa2);
    
    ToneGenerator* tg = new ToneGenerator(48000);
    audio_hub->add_processor(tg);
    
    dev->start();
    
    sleep(2);
    plpf->enable(false);
    sleep(2);
    plpf->enable(true);
    sleep(2);
    plpf->enable(false);
    sleep(2);
    plpf->enable(true);
    sleep(2);
    plpf->enable(false);
    sleep(2);
    plpf->enable(true);
    
    getchar();
    dev->stop();
    
    dev->close();
    
    delete dev_db;
    printf("Goodbye.\n");
	return 0;
}

#include <stdio.h>
#include <errno.h>
#include <getopt.h>

#include <string>

#include <asoundlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "device_db.h"
#include "pcm_device.h"
#include "pcm_playback.h"

#include "audio_params.h"
#include "command_handler.h"

#include "processor_simple_gain.h"
#include "processor_analyzer.h"
#include "processor_slow_agc.h"
#include "processor_lpf.h"
#include "tone_generator.h"
#include "soundfile.h"

#define INPUT_FILE_OPT 1000

static bool g_use_input_file = false;
static string g_input_filename;

using namespace std;

static struct option long_options[] = 
{
    {"verbose",    no_argument,       0,  'v' },
    {"input_file", required_argument, 0,  INPUT_FILE_OPT},
    {0, 0, 0, 0}
};

int process_cmd_line(int argc, char* argv[])
{
    int c;
    int option_index = 0;
    
    while (1) 
    {
        c = getopt_long(argc, argv, "v", long_options, &option_index);
                     
        if (c == -1)
        {
            break;
        }

        switch (c) 
        {
            case INPUT_FILE_OPT:
                cout << "input file: " << optarg << endl;
                g_use_input_file = true;
                g_input_filename = string(optarg);
                break;
            
            case 'v':
                printf("option v: verbose\n");
                break;

            case '?':
                return -EINVAL;

            default:
                printf("?? getopt returned character code 0%o ??\n", c);
                return -EINVAL;
        }
    }

    if (optind < argc) 
    {
        printf("non-option ARGV-elements: ");
        while (optind < argc)
        {
            printf("%s ", argv[optind++]);
        }
        printf("\n");
    }
    
    return 0;
}

int main(int argc, char* argv[])
{
    try
    {
        
    int retval = 0;
    retval = process_cmd_line(argc, argv);
    
    if(0 > retval)
    {
        return retval;
    }
    
    DeviceDb* dev_db = new DeviceDb();
    size_t num_devs = dev_db->find_bi_devices();
    cout << "Found " << num_devs << " bi device(s)." << endl;
    
    if(0 == num_devs)
    {
        cout << "Did not find any devices capable of playing back and recording (bi-directional devices).  Exiting." << endl;
        return -ENODEV;
    }
    
    string* dev_str = 0;
    dev_db->get_device_name(0, &dev_str);
    cout << "PCM device to be used: " << *dev_str << endl;
    
    AudioHub* audio_hub = new AudioHub();
    CommandHandler* command_handler = new CommandHandler();
    
    PCM_Device* dev = new PCM_Device(dev_str);
    if(true == g_use_input_file)
    {
        dev->set_capture_mode_stream();
        dev->set_capture_stream(g_input_filename);
    }
    
    dev->open();
    dev->set_audio_hub(audio_hub);
    
    audio_params_t audio_params;
    audio_params.num_chans = 2;
    audio_params.samp_rate = 48000;
    audio_params.frame_size = 8;
    
    //ProcessorSimpleGain* psg = new ProcessorSimpleGain(48000);
    //audio_hub->add_processor(psg);
    //psg->set_gain(-12.0, -12.0);
    
    
    
    //ProcessorLPF* plpf = new ProcessorLPF(48000);
    //audio_hub->add_processor(plpf);
    //ProcessorSlowAgc* psagc = new ProcessorSlowAgc(48000);
    //psagc->set_attack_release(2.0, 2.0);
    //psagc->set_target_lin(30000.0);
    //audio_hub->add_processor(psagc);
    
    //ProcessorAnalyzer* pa2 = new ProcessorAnalyzer(48000);
    //audio_hub->add_processor(pa2);
    
    ToneGenerator* tg = new ToneGenerator(&audio_params, "tone_gen");
    tg->set_frequency(440.0);
    tg->set_level(-6.0);
    tg->enable_channels(true, true);
    //audio_hub->add_processor(tg);
    //command_handler->add_processor(tg);
    
    ProcessorAnalyzer* pa1 = new ProcessorAnalyzer(&audio_params, "analyzer");
    audio_hub->add_processor(pa1);
    command_handler->add_processor(pa1);
    
    dev->start();
    
    command_handler->go();
    
    dev->stop();
    
    dev->close();
    
    delete command_handler;
    delete audio_hub;
    delete dev_db;
    
    printf("Goodbye.\n");
    
    }
    catch(runtime_error& err)
    {
        cout << "Exception caught: " << err.what() << endl;
    }
    
	return 0;
}

#include <stdio.h>

#include <string>

#include <asoundlib.h>

#include <string>
#include <vector>

using namespace std;

typedef struct
{
    snd_pcm_t* out;
    snd_pcm_t* in;
} PCM_DEV;

void get_card_names()
{
    int card_num = 0;
    int retval = 0;
    char* card_name = 0;
    
    do
    {
        retval = snd_card_get_name(card_num, &card_name);
        if(0 == retval)
        {
            printf("Card %d name is: %s\n", card_num, card_name);
            free(card_name);
            card_name = 0;
            snd_card_get_name(card_num, &card_name);
            printf("Card %d long name is: %s\n", card_num, card_name);
            free(card_name);
            card_num++;
        }
    } while(0 == retval);
}

void get_bi_pcm_devices(vector<string*>* bi_cards)
{
    int retval = -1;
    void** hints = 0;
    retval = snd_device_name_hint(-1, "pcm", &hints);
    
    if(0 > retval)
    {
        printf("Error %d getting ALSA PCM device name hints.\n", retval);
        return;
    }
    
    int hint_num = 0;
    void *hint = 0;
    
    string search_str("hw:");
    
    do
    {
        hint = hints[hint_num];
        
        if(NULL != hint)
        {
            char* name = snd_device_name_get_hint(hint, "NAME");
            if(NULL != name)
            {
                
                printf("Device %d NAME hint: %s\n", hint_num, name);
                char* ioid = snd_device_name_get_hint(hint, "IOID");
                if(NULL != ioid)
                {
                    printf("Device %d IOID hint: %s\n", hint_num, ioid);
                }
                else
                {
                    //this is a candidate bi-directional card that does 
                    //input and output.  Now find the 'hw' device...
                    string name_str(name);
                    size_t found = name_str.find(search_str);
                    
                    if(found == 0)
                    {
                        bi_cards->push_back(new string(name_str));
                    }
                }
            }
            hint_num++;
        }
        
    } while(NULL != hint);
    
    printf("Got %d ALSA device name hints.\n", hint_num);
    
    snd_device_name_free_hint(hints);
}

void setup_hardware(PCM_DEV* pcm_dev)
{   
    int retval = 0;
    snd_pcm_hw_params_t* hw_params = 0;
    snd_pcm_hw_params_malloc(&hw_params);
    
    snd_pcm_t* pcm_subdev = pcm_dev->out;
    
    retval = snd_pcm_hw_params_any(pcm_subdev, hw_params);
    if(0 > retval)
    {
        printf("Error %d getting PCM hw params for device: %s\n", retval, snd_strerror(retval));
        return;
    }
    
    retval = snd_pcm_hw_params_set_access(pcm_subdev, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if(0 > retval)
    {
        printf("Error %d setting PCM hw param 'access' for device: %s\n", retval, snd_strerror(retval));
        return;
    }
    
    retval = snd_pcm_hw_params_set_format(pcm_subdev, hw_params, SND_PCM_FORMAT_S16_LE);
    if(0 > retval)
    {
        printf("Error %d setting PCM hw param 'format' for device: %s\n", retval, snd_strerror(retval));
        return;
    }
    
    retval = snd_pcm_hw_params_set_channels(pcm_subdev, hw_params, 2);
    if(0 > retval)
    {
        printf("Error %d setting PCM hw param 'channels' for device: %s\n", retval, snd_strerror(retval));
        return;
    }
    
    retval = snd_pcm_hw_params_set_rate(pcm_subdev, hw_params, 48000, 0);
    if(0 > retval)
    {
        printf("Error %d setting PCM hw param 'rate' for device: %s\n", retval, snd_strerror(retval));
        return;
    }
    
    retval = snd_pcm_hw_params_set_rate_resample(pcm_subdev, hw_params, 0);
    if(0 > retval)
    {
        printf("Error %d setting PCM hw param 'rate_resample' for device: %s\n", retval, snd_strerror(retval));
        return;
    }
    
    retval = snd_pcm_hw_params(pcm_subdev, hw_params);
    if(0 > retval)
    {
        printf("Error %d setting PCM hw params for device: %s\n", retval, snd_strerror(retval));
        return;
    }
    
    printf("Finished setting HW params for device.\n");
    
    snd_pcm_uframes_t bufsize = 0;
    retval = snd_pcm_hw_params_get_buffer_size(hw_params, &bufsize);
    if(0 > retval)
    {
        printf("Error %d getting PCM hw params 'buffer_size' for device: %s\n", retval, snd_strerror(retval));
        return;
    }
    printf("PCM HW buffer size is %lu frames.\n", bufsize);
    
    unsigned int buftime = 0;
    retval = snd_pcm_hw_params_get_buffer_time(hw_params, &buftime, 0);
    if(0 > retval)
    {
        printf("Error %d getting PCM hw params 'buffer_time' for device: %s\n", retval, snd_strerror(retval));
        return;
    }
    
    printf("PCM HW buffer time is %d us.", buftime);
    
        
    snd_pcm_hw_params_free(hw_params);
    
    
}

void open_pcm(string* dev_name, PCM_DEV* pcm_dev)
{
    int retval = snd_pcm_open(&(pcm_dev->out), dev_name->c_str(), SND_PCM_STREAM_PLAYBACK, 0);
    if(0 > retval)
    {
        printf("Error %d opening PCM output device: %s\n", retval, snd_strerror(retval));
    }
    else
    {
        printf("PCM output device opened.\n");
    }
    
    retval = snd_pcm_open(&(pcm_dev->in), dev_name->c_str(), SND_PCM_STREAM_CAPTURE, 0);
    if(0 > retval)
    {
        printf("Error %d opening PCM input device: %s\n", retval, snd_strerror(retval));
    }
    else
    {
        printf("PCM input device opened.\n");
    }
}

void start_pcm(PCM_DEV* pcm_dev)
{
    
}

void stop_pcm(PCM_DEV* pcm_dev)
{
    
}

void close_pcm(PCM_DEV* pcm_dev)
{
    printf("Closing PCM devices.\n");
    snd_pcm_close(pcm_dev->in);
    snd_pcm_close(pcm_dev->out);
}

int main(int argc, char **argv)
{
    PCM_DEV* pcm_dev = new PCM_DEV();
    
    vector<string*> bi_cards;
	printf("Hello.\n");
    get_card_names();
    get_bi_pcm_devices(&bi_cards);
    
    vector<string*>::iterator it;
    
    printf("Bi-Directional PCM Cards Detected:\n");
    
    for(it = bi_cards.begin(); it != bi_cards.end(); it++)
    {
        printf("%s\n", (*it)->c_str());
    }
    
    if(0 == bi_cards.size())
    {
        printf("No bi-directional PCM cards detected.\n");
    }
    else
    {
        open_pcm(bi_cards.at(0), pcm_dev);
        setup_hardware(pcm_dev);
        start_pcm(pcm_dev);
        stop_pcm(pcm_dev);
        close_pcm(pcm_dev);
    }
    
    printf("Goodbye.\n");
	return 0;
}

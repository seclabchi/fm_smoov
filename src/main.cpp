#include <stdio.h>

#include <string>

#include <asoundlib.h>

#include <string>
#include <vector>

#define BUFFER_SIZE 16384
#define PERIOD_SIZE 4800

#define BUFFER_SIZE_BYTES BUFFER_SIZE * sizeof(int16_t) * 2

using namespace std;

int16_t buf_out[BUFFER_SIZE_BYTES];

typedef struct
{
    snd_pcm_t* out;
    snd_pcm_uframes_t bufsize_out;
    snd_pcm_uframes_t persize_out;
    snd_pcm_t* in;
    snd_pcm_uframes_t bufsize_in;
    snd_pcm_uframes_t persize_in;
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
    
    string search_str("sysdefault:");
    
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
    
    snd_pcm_uframes_t buf_size_tmp = BUFFER_SIZE;
    retval = snd_pcm_hw_params_set_buffer_size_near(pcm_subdev, hw_params, &buf_size_tmp);
    if(0 > retval)
    {
        printf("Error %d setting PCM hw param 'buffer_size' for device: %s\n", retval, snd_strerror(retval));
        return;
    }
    
    snd_pcm_uframes_t per_size_tmp = PERIOD_SIZE;
    retval = snd_pcm_hw_params_set_period_size_near(pcm_subdev, hw_params, &per_size_tmp, 0);
    if(0 > retval)
    {
        printf("Error %d setting PCM hw param 'period_size' for device: %s\n", retval, snd_strerror(retval));
        return;
    }
    
    snd_pcm_sw_params_t* sw_params = 0;
    snd_pcm_sw_params_malloc(&sw_params);
    snd_pcm_sw_params_current(pcm_subdev, sw_params);
        
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
    
    printf("PCM HW buffer time is %d us.\n", buftime);
    
    snd_pcm_uframes_t persize = 0;
    retval = snd_pcm_hw_params_get_period_size(hw_params, &persize, 0);
    if(0 > retval)
    {
        printf("Error %d getting PCM hw params 'period_size' for device: %s\n", retval, snd_strerror(retval));
        return;
    }
    printf("PCM HW period size is %lu frames.\n", persize);
    
    unsigned int pertime = 0;
    retval = snd_pcm_hw_params_get_period_time(hw_params, &pertime, 0);
    if(0 > retval)
    {
        printf("Error %d getting PCM hw params 'period_time' for device: %s\n", retval, snd_strerror(retval));
        return;
    }
    
    printf("PCM HW period time is %d us.\n", pertime);
    
        
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
    printf("Starting PCM...\n");
    snd_pcm_prepare(pcm_dev->in);
    snd_pcm_prepare(pcm_dev->out);
    snd_pcm_writei (pcm_dev->out, buf_out, 2 * PERIOD_SIZE);
    snd_pcm_start(pcm_dev->out);
    printf("PCM started.\n");
}

void stop_pcm(PCM_DEV* pcm_dev)
{
    printf("Stopping PCM...\n");
    snd_pcm_drop(pcm_dev->in);
    snd_pcm_drain(pcm_dev->out);
    printf("PCM stopped.\n");
}

void close_pcm(PCM_DEV* pcm_dev)
{
    printf("Closing PCM...\n");
    snd_pcm_close(pcm_dev->in);
    snd_pcm_close(pcm_dev->out);
    printf("PCM closed.\n");
}

int wait_for_poll(snd_pcm_t *handle, struct pollfd *ufds, unsigned int count)
{
    unsigned short revents;
    while (1) 
    {
        poll(ufds, count, -1);
        snd_pcm_poll_descriptors_revents(handle, ufds, count, &revents);
        if (revents & POLLERR)
                return -EIO;
        if (revents & POLLOUT)
                return 0;
    }
}

/*
 *   Underrun and suspend recovery
 */
 
int xrun_recovery(snd_pcm_t *handle, int err)
{
        printf("stream recovery\n");
        if (err == -EPIPE) {    /* under-run */
                err = snd_pcm_prepare(handle);
                if (err < 0)
                        printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
                return 0;
        } else if (err == -ESTRPIPE) {
                while ((err = snd_pcm_resume(handle)) == -EAGAIN)
                        sleep(1);       /* wait until the suspend flag is released */
                if (err < 0) {
                        err = snd_pcm_prepare(handle);
                        if (err < 0)
                                printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
                }
                return 0;
        }
        return err;
}

int write_and_poll_loop(snd_pcm_t *handle)
{
        struct pollfd *ufds;
        signed short *ptr;
        int err, count, cptr, init;
        int channels = 2;
        count = snd_pcm_poll_descriptors_count (handle);
        if (count <= 0) {
                printf("Invalid poll descriptors count\n");
                return count;
        }
        ufds = (struct pollfd*)malloc(sizeof(struct pollfd) * count);
        if (ufds == (struct pollfd*)NULL) {
                printf("No enough memory\n");
                return -ENOMEM;
        }
        if ((err = snd_pcm_poll_descriptors(handle, ufds, count)) < 0) {
                printf("Unable to obtain poll descriptors for playback: %s\n", snd_strerror(err));
                return err;
        }
        init = 1;
        while (1) {
                if (!init) {
                        err = wait_for_poll(handle, ufds, count);
                        if (err < 0) {
                                if (snd_pcm_state(handle) == SND_PCM_STATE_XRUN ||
                                    snd_pcm_state(handle) == SND_PCM_STATE_SUSPENDED) {
                                        err = snd_pcm_state(handle) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
                                        if (xrun_recovery(handle, err) < 0) {
                                                printf("Write error: %s\n", snd_strerror(err));
                                                exit(EXIT_FAILURE);
                                        }
                                        init = 1;
                                } else {
                                        printf("Wait for poll failed\n");
                                        return err;
                                }
                        }
                }
                ptr = buf_out;
                cptr = PERIOD_SIZE;
                while (cptr > 0) {
                        err = snd_pcm_writei(handle, ptr, cptr);
                        if (err < 0) {
                                if (xrun_recovery(handle, err) < 0) {
                                        printf("Write error: %s\n", snd_strerror(err));
                                        exit(EXIT_FAILURE);
                                }
                                init = 1;
                                break;  /* skip one period */
                        }
                        if (snd_pcm_state(handle) == SND_PCM_STATE_RUNNING)
                                init = 0;
                        ptr += err * channels;
                        cptr -= err;
                        if (cptr == 0)
                                break;
                        /* it is possible that the initial buffer cannot store */
                        /* all data from the last period, so wait awhile */
                        err = wait_for_poll(handle, ufds, count);
                        if (err < 0) {
                                if (snd_pcm_state(handle) == SND_PCM_STATE_XRUN ||
                                    snd_pcm_state(handle) == SND_PCM_STATE_SUSPENDED) {
                                        err = snd_pcm_state(handle) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
                                        if (xrun_recovery(handle, err) < 0) {
                                                printf("Write error: %s\n", snd_strerror(err));
                                                exit(EXIT_FAILURE);
                                        }
                                        init = 1;
                                } else {
                                        printf("Wait for poll failed\n");
                                        return err;
                                }
                        }
                }
        }
}

int main(int argc, char **argv)
{
    PCM_DEV* pcm_dev = new PCM_DEV();
    
    for(int16_t* ptr = buf_out; ptr < buf_out + BUFFER_SIZE_BYTES; ptr++)
    {
        *ptr = (int16_t)(((int32_t)(rand()) % 65535) - 32767);
    }
    
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
        write_and_poll_loop(pcm_dev->out);
        stop_pcm(pcm_dev);
        close_pcm(pcm_dev);
    }
    printf("Enter any key to quit.\n");
    getchar();
    printf("Goodbye.\n");
	return 0;
}

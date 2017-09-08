#include <stdio.h>

#include <string>

#include <asoundlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "device_db.h"
#include "pcm_device.h"

using namespace std;

/*
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
 
//
//int xrun_recovery(snd_pcm_t *handle, int err)
//{
//        printf("stream recovery\n");
//        if (err == -EPIPE) {    /* under-run */
//                err = snd_pcm_prepare(handle);
//                if (err < 0)
//                        printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
//                return 0;
//        } else if (err == -ESTRPIPE) {
//                while ((err = snd_pcm_resume(handle)) == -EAGAIN)
//                        sleep(1);       /* wait until the suspend flag is released */
//                if (err < 0) {
//                        err = snd_pcm_prepare(handle);
//                        if (err < 0)
//                                printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
//                }
//                return 0;
//        }
//        return err;
//}
//
//int write_and_poll_loop(snd_pcm_t *handle)
//{
//        struct pollfd *ufds;
//        signed short *ptr;
//        int err, count, cptr, init;
//        int channels = 2;
//        count = snd_pcm_poll_descriptors_count (handle);
//        if (count <= 0) {
//                printf("Invalid poll descriptors count\n");
//                return count;
//        }
//        ufds = (struct pollfd*)malloc(sizeof(struct pollfd) * count);
//        if (ufds == (struct pollfd*)NULL) {
//                printf("No enough memory\n");
//                return -ENOMEM;
//        }
//        if ((err = snd_pcm_poll_descriptors(handle, ufds, count)) < 0) {
//                printf("Unable to obtain poll descriptors for playback: %s\n", snd_strerror(err));
//                return err;
//        }
//        init = 1;
//        while (1) {
//                if (!init) {
//                        err = wait_for_poll(handle, ufds, count);
//                        if (err < 0) {
//                                if (snd_pcm_state(handle) == SND_PCM_STATE_XRUN ||
//                                    snd_pcm_state(handle) == SND_PCM_STATE_SUSPENDED) {
//                                        err = snd_pcm_state(handle) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
//                                        if (xrun_recovery(handle, err) < 0) {
//                                                printf("Write error: %s\n", snd_strerror(err));
//                                                exit(EXIT_FAILURE);
//                                        }
//                                        init = 1;
//                                } else {
//                                        printf("Wait for poll failed\n");
//                                        return err;
//                                }
//                        }
//                }
//                ptr = buf_out;
//                cptr = PERIOD_SIZE;
//                while (cptr > 0) {
//                        err = snd_pcm_writei(handle, ptr, cptr);
//                        if (err < 0) {
//                                if (xrun_recovery(handle, err) < 0) {
//                                        printf("Write error: %s\n", snd_strerror(err));
//                                        exit(EXIT_FAILURE);
//                                }
//                                init = 1;
//                                break;  /* skip one period */
//                        }
//                        if (snd_pcm_state(handle) == SND_PCM_STATE_RUNNING)
//                                init = 0;
//                        ptr += err * channels;
//                        cptr -= err;
//                        if (cptr == 0)
//                                break;
//                        /* it is possible that the initial buffer cannot store */
//                        /* all data from the last period, so wait awhile */
//                        err = wait_for_poll(handle, ufds, count);
//                        if (err < 0) {
//                                if (snd_pcm_state(handle) == SND_PCM_STATE_XRUN ||
//                                    snd_pcm_state(handle) == SND_PCM_STATE_SUSPENDED) {
//                                        err = snd_pcm_state(handle) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
//                                        if (xrun_recovery(handle, err) < 0) {
//                                                printf("Write error: %s\n", snd_strerror(err));
//                                                exit(EXIT_FAILURE);
//                                        }
//                                        init = 1;
//                                } else {
//                                        printf("Wait for poll failed\n");
//                                        return err;
//                                }
//                        }
//                }
//        }
//}

int main(int argc, char **argv)
{
    DeviceDb* dev_db = new DeviceDb();
    size_t num_devs = dev_db->find_bi_devices();
    cout << "Found " << num_devs << " bi device(s)." << endl;
    string* dev_str = 0;
    dev_db->get_device_name(0, &dev_str);
    cout << "PCM device to be used: " << *dev_str << endl;
    
    PCM_Device* dev = new PCM_Device(dev_str);
    dev->open();
    dev->close();
    
 /*   PCM_DEV* pcm_dev = new PCM_DEV();
    
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
    */
    
    delete dev_db;
    cout << "Press enter key to quit." << endl;
    getchar();
    printf("Goodbye.\n");
	return 0;
}

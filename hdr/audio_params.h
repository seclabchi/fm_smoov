#ifndef __AUDIO_PARAMS_H__
#define __AUDIO_PARAMS_H__

typedef struct
{
    uint32_t samp_rate;
    uint8_t num_chans;
    uint8_t frame_size;
} audio_params_t;

#endif /* __AUDIO_PARAMS_H__ */
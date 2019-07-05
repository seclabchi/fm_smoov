#ifndef __AUDIO_BUFFER_H__
#define __AUDIO_BUFFER_H__

#include <unistd.h>
#include <string.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <semaphore.h>

#include "biquad.h"
#include "compressor.h"
#include "gate.h"

#define BUFSIZE 384000
#define BUFSIZE_BAND BUFSIZE

#define DEBUG_CAPTURE

class AudioBuffer
{
public:
	static AudioBuffer* get_instance();
	virtual ~AudioBuffer();
	snd_pcm_sframes_t write_frames(float* ptr, snd_pcm_uframes_t num_w);
	snd_pcm_sframes_t read_frames(float* ptr, snd_pcm_uframes_t num_r);

	snd_pcm_uframes_t frames_avail();

	void wait_for_capture_start();


private:
	AudioBuffer();
	static AudioBuffer* the_instance;

	float buf[BUFSIZE];
	float* pRead;
	float* pWrite;
	float* pProc;
	float* bufend;
	snd_pcm_uframes_t num_write_frames_til_wrap;
	snd_pcm_uframes_t num_read_frames_til_wrap;
	snd_pcm_uframes_t pb_frames_avail;
	pthread_mutex_t the_mutex;

	uint32_t tmp_counter_w;
	uint32_t tmp_counter_r;

	FILE* capfile;

	void process_samples(float* p, uint32_t samps);

	float A, K, sig_pwr, z, e, g, g_prev, out_l, out_r, tmp_pwr;

	uint32_t sc, det_window;

	Biquad* filt_b1, *filt_b2, *filt_b3, *filt_b4, *filt_b5;
	float buf_b1[BUFSIZE_BAND];
	float buf_b2[BUFSIZE_BAND];
	float buf_b3[BUFSIZE_BAND];
	float buf_b4[BUFSIZE_BAND];
	float buf_b5[BUFSIZE_BAND];

	Compressor* compressor1, *compressor2, *compressor3, *compressor4, *compressor5;
	Gate* gate1, *gate2, *gate3, *gate4, *gate5;

	float lastpow1_l, lastpow1_r, lastpow2_l, lastpow2_r, lastpow3_l, lastpow3_r;
	float lastpow4_l, lastpow4_r, lastpow5_l, lastpow5_r;

	float peak_lin, peak_lin_db = 0;
	float peak_rin, peak_rin_db = 0;
	float rms_lin, rms_lin_db = 0;
	float rms_rin, rms_rin_db = 0;

	float peak_lout, peak_lout_db = 0;
	float peak_rout, peak_rout_db = 0;
	float rms_lout, rms_lout_db = 0;
	float rms_rout, rms_rout_db = 0;

	float absp = 0;

};

#endif

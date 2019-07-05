#include "audio_buffer.h"
#include <iostream>
#include <math.h>
#include <string.h>


using namespace std;

AudioBuffer* AudioBuffer::the_instance = NULL;

AudioBuffer* AudioBuffer::get_instance()
{
	if(NULL == the_instance)
	{
		the_instance = new AudioBuffer();
	}

	return the_instance;
}

AudioBuffer::AudioBuffer()
{
	memset(buf, 0, BUFSIZE * sizeof(float));
	bufend = buf + BUFSIZE;
	pRead = buf;
	pWrite = buf;
	pProc = buf;

	tmp_counter_w = 0;
	tmp_counter_r = 0;

	pthread_mutex_init(&the_mutex, NULL);

	//64 Hz LP filter
	filt_b1 = new Biquad(0.011850, 1, 1, 0, 1, -0.97630, 0);

	//64-362 Hz BP
	filt_b2 = new Biquad(0.052889, 1, 0, -1, 1, -1.8938, 0.89422);

	//362-2040 Hz BP
	filt_b3 = new Biquad(0.23993, 1, 0, -1, 1, -1.5105, 0.52013);

	//2040-11700 Hz BP
	filt_b4 = new Biquad(0.67712, 1, 0, -1, 1, -0.49804, -0.35425);

	//11700-15000 Hz BP
	filt_b5 = new Biquad(0.38579, 1, 0, -1, 1, 0.22109, 0.22842);

	gate1 = new Gate();
	gate2 = new Gate();
	gate3 = new Gate();
	gate4 = new Gate();
	gate5 = new Gate();

	compressor1 = new Compressor();  //<64 Hz  BAND
	compressor2 = new Compressor();  //64-362 Hz BAND
	compressor3 = new Compressor();  //362-2040 Hz BAND
	compressor4 = new Compressor();  //2040-11700 Hz BAND
	compressor5 = new Compressor();  //11700-15000 Hz BAND




#ifdef DEBUG_CAPTURE
	capfile = fopen("/tmp/capturetmp.pcm", "wb");
#endif
}

AudioBuffer::~AudioBuffer()
{
#ifdef DEBUG_CAPTURE
	fclose(capfile);
#endif
}

snd_pcm_sframes_t AudioBuffer::write_frames(float* ptr, snd_pcm_uframes_t num_w)
{
	//pthread_mutex_lock(&the_mutex);

	uint32_t samples_to_write = num_w * 2;

	uint32_t space_left = (bufend-pWrite);

	//cout << "IN: samples_to_write=" << samples_to_write << ", space_left=" << space_left << endl;

	if(space_left >= samples_to_write)
	{
		memcpy(pWrite, ptr, samples_to_write * sizeof(float));
		process_samples(pWrite, samples_to_write);
		pWrite += samples_to_write;
	}
	else
	{
		memcpy(pWrite, ptr, space_left * sizeof(float));
		process_samples(pWrite, space_left);
		samples_to_write -= space_left;
		pWrite = buf;
		//cout << "WRITE WRAP" << endl;
		memcpy(pWrite, ptr + space_left, samples_to_write * sizeof(float));
		process_samples(pWrite, samples_to_write);
		pWrite += samples_to_write;
	}

	return num_w;
}

void AudioBuffer::process_samples(float* p, uint32_t samps)
{
	if(samps > 0)
	{
		peak_lin = 0;
		peak_rin = 0;
		rms_lin = 0;
		rms_rin = 0;
		//COMPUTE PRE-PROCESSED SIGNAL INFO
		for(uint32_t i = 0; i < samps; i+=2)
		{
			rms_lin += powf(p[i], 2);
			rms_rin += powf(p[i+1], 2);

			absp = fabsf(p[i]);

			if(absp > peak_lin)
			{
				peak_lin = absp;
			}

			absp = fabsf(p[i+1]);

			if(absp > peak_rin)
			{
				peak_rin = absp;
			}
		}

		rms_lin = sqrtf(2.0 * rms_lin / samps);
		rms_rin = sqrtf(2.0 * rms_rin / samps);

		if(rms_lin > 0)
		{
			rms_lin_db = 20 * log10(rms_lin);
		}
		else
		{
			rms_lin_db = -99;
		}

		if(rms_rin > 0)
		{
			rms_rin_db = 20 * log10(rms_rin);
		}
		else
		{
			rms_rin_db = -99;
		}

		if(peak_lin > 0)
		{
			peak_lin_db = 20 * log10(peak_lin);
		}
		else
		{
			peak_lin_db = -99;
		}

		if(peak_rin > 0)
		{
			peak_rin_db = 20 * log10(peak_rin);
		}
		else
		{
			peak_rin_db = -99;
		}

		cout << "IN" << " samps: " << samps << ", Peak/RMS L: " << peak_lin_db << "," << rms_lin_db << " - Peak/RMS R: " << peak_rin_db << "," << rms_rin_db << endl;

		//EXPERIMENTAL STEREO WIDENING START
		float ses = 4.0;
		float M, S;

		for(int i = 0; i < samps; i += 2)
		{
			M = ses * (p[i] + p[i+1]) / 2.0;
			S = ses * (p[i] - p[i+1]) / 2.0;

			p[i] = 1/ses * (M + S);
			p[i+1] = 1/ses * (M - S);
		}

		//EXPERIMENTAL STEREO WIDENING END

		memcpy(buf_b1, p, samps * sizeof(float));
		memcpy(buf_b2, p, samps * sizeof(float));
		memcpy(buf_b3, p, samps * sizeof(float));
		memcpy(buf_b4, p, samps * sizeof(float));
		memcpy(buf_b5, p, samps * sizeof(float));

		filt_b1->process(buf_b1, samps);
		filt_b2->process(buf_b2, samps);
		filt_b3->process(buf_b3, samps);
		filt_b4->process(buf_b4, samps);
		filt_b5->process(buf_b5, samps);

//		gate1->process(buf_b1, samps);
//		gate2->process(buf_b2, samps);
//		gate3->process(buf_b3, samps);
//		gate4->process(buf_b4, samps);
//		gate5->process(buf_b5, samps);

		compressor1->process(buf_b1, samps);
		compressor2->process(buf_b2, samps);
		compressor3->process(buf_b3, samps);
		compressor4->process(buf_b4, samps);
		compressor5->process(buf_b5, samps);

	//	compressor1->get_last_power(&lastpow1_l, &lastpow1_r);
	//	compressor2->get_last_power(&lastpow2_l, &lastpow2_r);
	//	compressor3->get_last_power(&lastpow3_l, &lastpow3_r);
	//	compressor4->get_last_power(&lastpow4_l, &lastpow4_r);
	//	compressor5->get_last_power(&lastpow5_l, &lastpow5_r);

	//	cout << lastpow1_l << " " << lastpow1_r << endl;
	//	cout << lastpow2_l << " " << lastpow2_r << endl;
	//	cout << lastpow3_l << " " << lastpow3_r << endl;
	//	cout << lastpow4_l << " " << lastpow4_r << endl;
	//	cout << lastpow5_l << " " << lastpow5_r << endl;

		#define GAIN_ADJ .17

		peak_lout = 0;
		peak_rout = 0;
		rms_lout = 0;
		rms_rout = 0;

		for(uint32_t i = 0; i < samps; i+=2)
		{
			p[i] = GAIN_ADJ * buf_b1[i] + GAIN_ADJ * buf_b2[i] + GAIN_ADJ * buf_b3[i] + GAIN_ADJ * buf_b4[i] + GAIN_ADJ * buf_b5[i];
			p[i+1] = GAIN_ADJ * buf_b1[i+1] + GAIN_ADJ * buf_b2[i+1] + GAIN_ADJ * buf_b3[i+1] + GAIN_ADJ * buf_b4[i+1] + GAIN_ADJ * buf_b5[i+1];

			if(p[i] > 1.0)
			{
				p[i] = 1.0;
				cout << "CLIP L" << endl;
			}

			if(p[i+1] > 1.0)
			{
				p[i+1] = 1.0;
				cout << "CLIP R" << endl;
			}

			rms_lout += powf(p[i], 2);
			rms_rout += powf(p[i+1], 2);

			absp = fabsf(p[i]);

			if(absp > peak_lout)
			{
				peak_lout = absp;
			}

			absp = fabsf(p[i+1]);

			if(absp > peak_rout)
			{
				peak_rout = absp;
			}
		}

		rms_lout = sqrtf(2.0 * rms_lout / samps);
		rms_rout = sqrtf(2.0 * rms_rout / samps);

		if(rms_lout > 0)
		{
			rms_lout_db = 20 * log10(rms_lout);
		}
		else
		{
			rms_lout_db = -99;
		}

		if(rms_rout > 0)
		{
			rms_rout_db = 20 * log10(rms_rout);
		}
		else
		{
			rms_rout_db = -99;
		}

		if(peak_lout > 0)
		{
			peak_lout_db = 20 * log10(peak_lout);
		}
		else
		{
			peak_lout_db = -99;
		}

		if(peak_rout > 0)
		{
			peak_rout_db = 20 * log10(peak_rout);
		}
		else
		{
			peak_rout_db = -99;
		}

		cout << "OUT" << " samps: " << samps << ", Peak/RMS L: " << peak_lout_db << "," << rms_lout_db << " - Peak/RMS R: " << peak_rout_db << "," << rms_rout_db << endl;

#ifdef DEBUG_CAPTURE
		fwrite(p, sizeof(float), samps, capfile);
#endif
	}
	else
	{
		cout << "samps = 0!" << endl;
	}
}

snd_pcm_sframes_t AudioBuffer::read_frames(float* ptr, snd_pcm_uframes_t num_r)
{
	snd_pcm_sframes_t frames_read = 0;
	uint32_t samples_to_read = num_r * 2;
	uint32_t samples_read = 0;
	uint32_t outbuf_offset = 0;

	if(pRead + samples_to_read <= bufend)
	{
		memcpy(ptr, pRead, samples_to_read * sizeof(float));
		pRead += samples_to_read;
		samples_to_read = 0;
	}
	else
	{
		memcpy(ptr, pRead, (bufend-pRead) * sizeof(float));
		//cout << "READ WRAP" << endl;
		samples_to_read -= bufend-pRead;
		outbuf_offset = bufend-pRead;
		pRead = buf;
		memcpy(ptr + outbuf_offset, pRead, samples_to_read * sizeof(float));
		pRead += samples_to_read;
		samples_to_read = 0;
	}

	frames_read = num_r;

	return frames_read;
}

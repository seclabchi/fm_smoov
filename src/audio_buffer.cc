#include "audio_buffer.h"
#include <iostream>
#include <math.h>
#include <string.h>

using namespace std;

float AudioBuffer::peak_lin_db = 0;
float AudioBuffer::rms_lin_db = 0;
float AudioBuffer::peak_rin_db = 0;
float AudioBuffer::rms_rin_db = 0;
float AudioBuffer::peak_lout_db = 0;
float AudioBuffer::rms_lout_db = 0;
float AudioBuffer::peak_rout_db = 0;
float AudioBuffer::rms_rout_db = 0;

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

	bands = new float*[4];
	b1 = new float[BUFSIZE];
	b2 = new float[BUFSIZE];
	b3 = new float[BUFSIZE];
	b4 = new float[BUFSIZE];

	memset(b1, 0, sizeof(float)*BUFSIZE);
	memset(b2, 0, sizeof(float)*BUFSIZE);
	memset(b3, 0, sizeof(float)*BUFSIZE);
	memset(b4, 0, sizeof(float)*BUFSIZE);

	bands[0] = b1; // <153 Hz
	bands[1] = b2; // 153-860 Hz
	bands[2] = b3; // 860-4900 Hz
	bands[3] = b4; // >4900 Hz

	crossover = new Crossover(BUFSIZE, bands);

//	gate1 = new Gate();
//	gate2 = new Gate();
//	gate3 = new Gate();
//	gate4 = new Gate();
//	gate5 = new Gate();

	//AGC setup: TL, GT, Tatt, Trel
	input_agc = new AGC(-15.0, 25.0, .250, .250);

	//Compressor setup: _R, _T, _G, _W, _Tatt, _Trel
	compressor1 = new Compressor(5.0, -15.0, 3.5, 0.5, 0.050, 0.100);
	compressor2 = new Compressor(8.0, -20.0, 1.5, 0.5, 0.010, 0.100);
	compressor3 = new Compressor(8.0, -23.0, 1.5, 0.5, 0.0010, 0.100);
	compressor4 = new Compressor(9.0, -23.0, 2.0, 0.5, 0.00001, 0.075);


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
		//cout << samps << "samps" << endl;
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

		//Do slow AGC on the input
		input_agc->process(p, samps);

		//EXPERIMENTAL STEREO WIDENING START

		float ses = 1.5;
		float M, S;

		for(int i = 0; i < samps; i += 2)
		{
			M = ses * (p[i] + p[i+1]) / 2.0;
			S = ses * (p[i] - p[i+1]) / 2.0;

			p[i] = 1/ses * (M + S);
			p[i+1] = 1/ses * (M - S);
		}

		//EXPERIMENTAL STEREO WIDENING END

		crossover->process(p, samps);

//		gate1->process(buf_b1, samps);
//		gate2->process(buf_b2, samps);
//		gate3->process(buf_b3, samps);
//		gate4->process(buf_b4, samps);

		compressor1->process(b1, samps);
		compressor2->process(b2, samps);
		compressor3->process(b3, samps);
		compressor4->process(b4, samps);

		peak_lout = 0;
		peak_rout = 0;
		rms_lout = 0;
		rms_rout = 0;

		float b1_gain = .20;
		float b2_gain = .20;
		float b3_gain = .20;
		float b4_gain = .20;

		for(uint32_t i = 0; i < samps; i+=2)
		{
			p[i] = b1_gain * b1[i] + b2_gain * b2[i] + b3_gain * b3[i] + b4_gain * b4[i];

			p[i+1] = b1_gain * b1[i+1] + b2_gain * b2[i+1] + b3_gain * b3[i+1] + b4_gain * b4[i+1];

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

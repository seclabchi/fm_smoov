#include "sine_wave_gen.h"

#include <math.h>
#include <cstdlib>

#define PI 3.14159265359

SineWaveGen::SineWaveGen(uint32_t _samprate, uint32_t _tonefreq, bool _floating, uint8_t _bitdepth)
{
	samprate = _samprate;
	tonefreq = _tonefreq;
	floating = _floating;
	bitdepth = _bitdepth;
	tonefreq_omega = 2.0f * PI * (float)tonefreq;
	current_pos = 0;
	if(true == floating)
	{
		sine_amp = 1.0;
	}
	else
	{
		sine_amp = (float)(pow(2.0f, bitdepth)) / 6.0;
	}
}

float SineWaveGen::get_next_value()
{
	float inval = (float)current_pos / (float)samprate;

	float outval = sine_amp * sin(tonefreq_omega * inval);

	current_pos++;
	if(samprate == current_pos)
	{
		current_pos = 0;
	}

	//outval = (rand() % ((uint32_t)sine_amp * 2)) - sine_amp;

	return outval;
}

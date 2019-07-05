#include <stdint.h>
#include <unistd.h>

/* This class assumes some form of either signed integer sample format, or floating point format. */

class SineWaveGen
{
public:
	SineWaveGen(uint32_t _samprate, uint32_t _tonefreq, bool _floating, uint8_t _bitdepth = 0);
	float get_next_value();

private:
	uint32_t samprate;
	uint32_t tonefreq;
	bool floating;
	uint8_t bitdepth;

	float tonefreq_omega;
	uint32_t current_pos;
	float sine_amp;
};

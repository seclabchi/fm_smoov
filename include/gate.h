#ifndef __GATE_H__
#define __GATE_H__

#include <stdint.h>

class Gate
{
public:
	Gate();
	virtual ~Gate();
	void process(float* p, uint32_t samps);

private:
	float timeconst_a; //attack time constant in seconds
	float timeconst_r; //release time constant in seconds
	float alpha_a; //attack time constant calc
	float alpha_r; //release time constant calc
	float T; //threshold in dB
	float Tlin; //threshold linear
	float HT;  //hold time
	uint32_t HTI; //hold time in samples
	uint32_t CaL, CaR;  //attack hold time counter
	uint32_t CrL, CrR;  //release hold time counter

	float gcL, gcR; //computed gain
	float gsL, gsR; //computed gain smoothing
	float gsLprev, gsRprev;  //previous gain smoothing value

	void recalculate();
};

#endif

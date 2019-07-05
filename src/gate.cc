#include "gate.h"
#include <math.h>

Gate::Gate()
{
	timeconst_a = 0.1;
	timeconst_r = 0.1;

	T = -50; //threshold in dB

	HT = 0.08;

	recalculate();
}

Gate::~Gate()
{

}

void Gate::recalculate()
{
	Tlin = powf(10, T/20.0);
	CaL = 1;
	CaR = 1;
	CrL = 1;
	CrR = 1;
	alpha_a = expf(-logf(9.0)/(48000.0 * timeconst_a));
	alpha_r = expf(-logf(9.0)/(48000.0 * timeconst_r));
	HTI = (uint32_t)(HT * 48000.0);
	gsLprev = 1.0;
	gsRprev = 1.0;
}

void Gate::process(float* p, uint32_t samps)
{
	float absL, absR;

	for(int i = 0; i < samps; i += 2)
	{
		absL = fabsf(p[i]);
		absR = fabsf(p[i+1]);

		if(absL < Tlin)
		{
			gcL = 0;
			CrL++;
			CaL = 1;
		}
		else
		{
			gcL = 1;
			CaL++;
			CrL = 1;
		}

		if(absR < Tlin)
		{
			gcR = 0;
			CrR++;
			CaR = 1;
		}
		else
		{
			gcR = 1;
			CaR++;
			CrR = 1;
		}

		if(gcL <= gsLprev)
		{
			if(CaL > HTI)
			{
				gsL = alpha_a * gsLprev + (1-alpha_a) * gcL;
			}
			else //CaL <= HTI
			{
				gsL = gsLprev;
			}
		}
		else //gcL > gsLprev
		{
			if(CrL > HTI)
			{
				gsL = alpha_r * gsLprev + (1-alpha_r) * gcL;
			}
			else //CaR <= HTI
			{
				gsL = gsLprev;
			}
		}

		gsLprev = gsL;

		if(gcR <= gsRprev)
		{
			if(CaR > HTI)
			{
				gsR = alpha_a * gsRprev + (1-alpha_a) * gcR;
			}
			else //CaR <= HTI
			{
				gsR = gsRprev;
				CaR++;
			}
		}
		else //gcL > gsLprev
		{
			if(CrR > HTI)
			{
				gsR = alpha_r * gsRprev + (1-alpha_r) * gcR;
			}
			else //CrR <= HTI
			{
				gsR = gsRprev;
				CrR++;
			}
		}

		gsRprev = gsR;

		p[i] = p[i] * gsL;
		p[i+1] = p[i+1] * gsR;

	}
}

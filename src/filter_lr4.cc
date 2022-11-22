#include <filter_lr4.h>
#include <string.h>
#include <iostream>


using namespace std;


FilterLR4::FilterLR4(SOS* _sos, uint32_t _bufsize) : m_bufsize(_bufsize), sos(_sos)
{
//  octave_stdout << "Creating new biquad with params " << s1->g << " " << s1->b0 << " " << s1->b1 << " " << s1->b2 << " " << s1->a1 << " " << s1->a2 << endl;
  bq1 = new Biquad(sos);
//  octave_stdout << "Creating new biquad with params " << s2->g << " " << s2->b0 << " " << s2->b1 << " " << s2->b2 << " " << s2->a1 << " " << s2->a2 << endl;
  bq2 = new Biquad(sos);
  tmpbuf1 = new float[m_bufsize];
  tmpbuf2 = new float[m_bufsize];
}

FilterLR4::~FilterLR4()
{
	delete bq1;
	delete bq2;
	delete[] tmpbuf1;
	delete[] tmpbuf2;
}

void FilterLR4::process(float* in, float* out)
{
	memcpy(tmpbuf1, in, m_bufsize * sizeof(float));
    bq1->process(tmpbuf1, m_bufsize);
    memcpy(tmpbuf2, tmpbuf1, m_bufsize * sizeof(float));
    bq2->process(tmpbuf2, m_bufsize);
	memcpy(out, tmpbuf2, m_bufsize * sizeof(float));
}

AllpassFilterLR4::AllpassFilterLR4(SOS* _soslo, SOS* _soshi, uint32_t _bufsize) : m_bufsize(_bufsize)
{
	filterL = new FilterLR4(_soslo, m_bufsize);
	filterH = new FilterLR4(_soshi, m_bufsize);
	tmpbufL = new float[m_bufsize];
	tmpbufH = new float[m_bufsize];
}

AllpassFilterLR4::~AllpassFilterLR4()
{
	delete filterL;
	delete filterH;
	delete[] tmpbufL;
	delete[] tmpbufH;
}

void AllpassFilterLR4::process(float* in, float* out)
{
	filterL->process(in, tmpbufL);
	filterH->process(in, tmpbufH);

	for(uint32_t i = 0; i < m_bufsize; i++)
	{
		out[i] = tmpbufL[i] + tmpbufH[i];
	}
}


/*
 * plugin_crossover.cpp
 *
 *  Created on: Oct 27, 2022
 *      Author: zaremba
 */

#include <plugin_crossover.h>

/*
 * SECOND-ORDER BUTTERWORTH SOS COEFFICIENTS FOR CROSSOVER FREQUENCIES (Hz)
 *
 * 153, 430, 1100, 2400, 5900
 *
 * SPLIT "BAND" ORDER FOR TREE STRUCTURE:
 *
 * b0 - 2400
 * b1 - 430
 * b2 - 5900
 * b3 - 153
 * b4 - 1100
 *
b0lo_sos =
   1.0000   2.0000   1.0000   1.0000  -1.5610   0.6414
g0lo = 0.020083

b0hi_sos =
   1.0000  -2.0000   1.0000   1.0000  -1.5610   0.6414
g0hi = 0.8006

b1lo_sos =
   1.0000   2.0000   1.0000   1.0000  -1.9204   0.9235
g1lo = 7.6155e-04

b1hi_sos =
   1.0000  -2.0000   1.0000   1.0000  -1.9204   0.9235
g1hi = 0.9610

b2lo_sos =
   1.0000   2.0000   1.0000   1.0000  -0.9593   0.3392
g2lo = 0.094983

b2hi_sos =
   1.0000  -2.0000   1.0000   1.0000  -0.9593   0.3392
g2hi = 0.5746

b3lo_sos =
   1.0000   2.0000   1.0000   1.0000  -1.9717   0.9721
g3lo = 9.8873e-05

b3hi_sos =
   1.0000  -2.0000   1.0000   1.0000  -1.9717   0.9721
g3hi = 0.9859

b4lo_sos =
   1.0000   2.0000   1.0000   1.0000  -1.7970   0.8158
g4lo = 4.6977e-03

b4hi_sos =
   1.0000  -2.0000   1.0000   1.0000  -1.7970   0.8158
g4hi = 0.9032

*/

PluginCrossover::PluginCrossover(const std::string& _name, uint32_t samprate, uint32_t bufsize) : ProcessorPlugin(_name.c_str(), samprate, bufsize) {
	/* Construct the second order sections */
	s0lo = new SOS(0.020083,   1.0000,  2.0000, 1.0000, 1.0000, -1.5610, 0.6414);
	s0hi = new SOS(0.8006,     1.0000, -2.0000, 1.0000, 1.0000, -1.5610, 0.6414);
	s1lo = new SOS(7.6155e-04, 1.0000,  2.0000, 1.0000, 1.0000, -1.9204, 0.9235);
	s1hi = new SOS(0.9610,     1.0000, -2.0000, 1.0000, 1.0000, -1.9204, 0.9235);
	s2lo = new SOS(0.094983,   1.0000,  2.0000, 1.0000, 1.0000, -0.9593, 0.3392);
	s2hi = new SOS(0.5746,     1.0000, -2.0000, 1.0000, 1.0000, -0.9593, 0.3392);
	s3lo = new SOS(9.8873e-05, 1.0000,  2.0000, 1.0000, 1.0000, -1.9717, 0.9721);
	s3hi = new SOS(0.9859,     1.0000, -2.0000, 1.0000, 1.0000, -1.9717, 0.9721);
	s4lo = new SOS(4.6977e-03, 1.0000,  2.0000, 1.0000, 1.0000, -1.7970, 0.8158);
	s4hi = new SOS(0.9032,     1.0000, -2.0000, 1.0000, 1.0000, -1.7970, 0.8158);

	/* Create the low/high pass filters */
	f0loL = new FilterLR4(s0lo, m_bufsize);
	f0loR = new FilterLR4(s0lo, m_bufsize);
	f0hiL = new FilterLR4(s0hi, m_bufsize);
	f0hiR = new FilterLR4(s0hi, m_bufsize);
	f1loL = new FilterLR4(s1lo, m_bufsize);
	f1loR = new FilterLR4(s1lo, m_bufsize);
	f1hiL = new FilterLR4(s1hi, m_bufsize);
	f1hiR = new FilterLR4(s1hi, m_bufsize);
	f2loL = new FilterLR4(s2lo, m_bufsize);
	f2loR = new FilterLR4(s2lo, m_bufsize);
	f2hiL = new FilterLR4(s2hi, m_bufsize);
	f2hiR = new FilterLR4(s2hi, m_bufsize);
	f3loL = new FilterLR4(s3lo, m_bufsize);
	f3loR = new FilterLR4(s3lo, m_bufsize);
	f3hiL = new FilterLR4(s3hi, m_bufsize);
	f3hiR = new FilterLR4(s3hi, m_bufsize);
	f4loL = new FilterLR4(s4lo, m_bufsize);
	f4loR = new FilterLR4(s4lo, m_bufsize);
	f4hiL = new FilterLR4(s4hi, m_bufsize);
	f4hiR = new FilterLR4(s4hi, m_bufsize);

	/* Create the allpass filters */
	fa0p2L = new AllpassFilterLR4(s2lo, s2hi, m_bufsize);
	fa0p2R = new AllpassFilterLR4(s2lo, s2hi, m_bufsize);
	fa0q1L = new AllpassFilterLR4(s1lo, s1hi, m_bufsize);
	fa0q1R = new AllpassFilterLR4(s1lo, s1hi, m_bufsize);
	fa0q3L = new AllpassFilterLR4(s3lo, s3hi, m_bufsize);
	fa0q3R = new AllpassFilterLR4(s3lo, s3hi, m_bufsize);
	fa0q4L = new AllpassFilterLR4(s4lo, s4hi, m_bufsize);
	fa0q4R = new AllpassFilterLR4(s4lo, s4hi, m_bufsize);
	fa1p4L = new AllpassFilterLR4(s4lo, s4hi, m_bufsize);
	fa1p4R = new AllpassFilterLR4(s4lo, s4hi, m_bufsize);
	fa1q3L = new AllpassFilterLR4(s3lo, s3hi, m_bufsize);
	fa1q3R = new AllpassFilterLR4(s3lo, s3hi, m_bufsize);

	f0lo_outL = new float[m_bufsize];
	f0lo_outR = new float[m_bufsize];
	a0p2_outL = new float[m_bufsize];
	a0p2_outR = new float[m_bufsize];
	f1lo_outL = new float[m_bufsize];
	f1lo_outR = new float[m_bufsize];
	fa1p4_outL = new float[m_bufsize];
	fa1p4_outR = new float[m_bufsize];
	f1hi_outL = new float[m_bufsize];
	f1hi_outR = new float[m_bufsize];
	fa1q3_outL = new float[m_bufsize];
	fa1q3_outR = new float[m_bufsize];
	f0hi_outL = new float[m_bufsize];
	f0hi_outR = new float[m_bufsize];
	a0q1_outL = new float[m_bufsize];
	a0q1_outR = new float[m_bufsize];
	a0q3_outL = new float[m_bufsize];
	a0q3_outR = new float[m_bufsize];
	a0q4_outL = new float[m_bufsize];
	a0q4_outR = new float[m_bufsize];
}

PluginCrossover::~PluginCrossover() {
	delete f0lo_outL;
	delete f0lo_outR;
	delete a0p2_outL;
	delete a0p2_outR;
	delete f1lo_outL;
	delete f1lo_outR;
	delete fa1p4_outL;
	delete fa1p4_outR;
	delete f1hi_outL;
	delete f1hi_outR;
	delete fa1q3_outL;
	delete fa1q3_outR;
	delete f0hi_outL;
	delete f0hi_outR;
	delete a0q1_outL;
	delete a0q1_outR;
	delete a0q3_outL;
	delete a0q3_outR;
	delete a0q4_outL;
	delete a0q4_outR;

	delete fa0p2L;
	delete fa0p2R;
	delete fa0q1L;
	delete fa0q1R;
	delete fa0q3L;
	delete fa0q3R;
	delete fa0q4L;
	delete fa0q4R;
	delete fa1p4L;
	delete fa1p4R;
	delete fa1q3L;
	delete fa1q3R;

	delete f0loL;
	delete f0loR;
	delete f0hiL;
	delete f0hiR;
	delete f1loL;
	delete f1loR;
	delete f1hiL;
	delete f1hiR;
	delete f2loL;
	delete f2loR;
	delete f2hiL;
	delete f2hiR;
	delete f3loL;
	delete f3loR;
	delete f3hiL;
	delete f3hiR;
	delete f4loL;
	delete f4loR;
	delete f4hiL;
	delete f4hiR;

	delete s0lo;
	delete s0hi;
	delete s1lo;
	delete s1hi;
	delete s2lo;
	delete s2hi;
	delete s3lo;
	delete s3hi;
	delete s4lo;
	delete s4hi;

}

int PluginCrossover::do_process() {

	/* Do band 0 chain (< 153 Hz) */
	f0loL->process(in_L, f0lo_outL);
	f0loR->process(in_R, f0lo_outR);
	fa0p2L->process(f0lo_outL, a0p2_outL);
	fa0p2R->process(f0lo_outR, a0p2_outR);
	f1loL->process(a0p2_outL, f1lo_outL);
	f1loR->process(a0p2_outR, f1lo_outR);
	fa1p4L->process(f1lo_outL, fa1p4_outL);
	fa1p4R->process(f1lo_outR, fa1p4_outR);
	f3loL->process(fa1p4_outL, b0_outL);
	f3loR->process(fa1p4_outR, b0_outR);

	/* Do band 1 chain (153 Hz - 430 Hz) */
	f3hiL->process(fa1p4_outL, b1_outL);
	f3hiR->process(fa1p4_outR, b1_outR);

	/* Do band 2 chain (430 Hz - 1.1 kHz) */
	f1hiL->process(a0p2_outL, f1hi_outL);
	f1hiR->process(a0p2_outR, f1hi_outR);
	fa1q3L->process(f1hi_outL, fa1q3_outL);
	fa1q3R->process(f1hi_outR, fa1q3_outR);
	f4loL->process(fa1q3_outL, b2_outL);
	f4loR->process(fa1q3_outR, b2_outR);

	/* Do band 3 chain (1.1 kHz - 2.4 kHz) */
	f4hiL->process(fa1q3_outL, b3_outL);
	f4hiR->process(fa1q3_outR, b3_outR);

	/* Do band 4 chain (2.4 kHz - 5.9 kHz) */
	f0hiL->process(in_L, f0hi_outL);
	f0hiR->process(in_R, f0hi_outR);
	fa0q1L->process(f0hi_outL, a0q1_outL);
	fa0q1R->process(f0hi_outR, a0q1_outR);
	fa0q3L->process(a0q1_outL, a0q3_outL);
	fa0q3R->process(a0q1_outR, a0q3_outR);
	fa0q4L->process(a0q3_outL, a0q4_outL);
	fa0q4R->process(a0q3_outR, a0q4_outR);
	f2loL->process(a0q4_outL, b4_outL);
	f2loR->process(a0q4_outR, b4_outR);

	/* Do band 5 chain (>5.9 kHz) */
	f2hiL->process(a0q4_outL, b5_outL);
	f2hiR->process(a0q4_outR, b5_outR);

	/* Provide 2 passthrough channels */
	for(uint32_t i = 0; i < m_bufsize; i++) {
		out_L[i] = b0_outL[i] + b1_outL[i] + b2_outL[i] + b3_outL[i] + b4_outL[i] + b5_outL[i];
		out_R[i] = b0_outR[i] + b1_outR[i] + b2_outR[i] + b3_outR[i] + b4_outR[i] + b5_outR[i];
	}

	return 0;
}

bool PluginCrossover::do_change_cfg(const fmsmoov::PluginConfig& cfg) {
	return true;
}

bool PluginCrossover::do_init(const fmsmoov::PluginConfig& cfg) {
	AudioBuf* btmp;

	btmp = new AudioBuf("CROSSOVER_B0_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b0_outL = btmp->get();

	btmp = new AudioBuf("CROSSOVER_B0_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b0_outR = btmp->get();

	btmp = new AudioBuf("CROSSOVER_B1_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b1_outL = btmp->get();

	btmp = new AudioBuf("CROSSOVER_B1_OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b1_outR = btmp->get();

	btmp = new AudioBuf("CROSSOVER_B2_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b2_outL = btmp->get();

	btmp = new AudioBuf("CROSSOVER_B2_OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b2_outR = btmp->get();

	btmp = new AudioBuf("CROSSOVER_B3_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b3_outL = btmp->get();

	btmp = new AudioBuf("CROSSOVER_B3_OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b3_outR = btmp->get();

	btmp = new AudioBuf("CROSSOVER_B4_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b4_outL = btmp->get();

	btmp = new AudioBuf("CROSSOVER_B4_OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b4_outR = btmp->get();

	btmp = new AudioBuf("CROSSOVER_B5_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b5_outL = btmp->get();

	btmp = new AudioBuf("CROSSOVER_B5_OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b5_outR = btmp->get();

	return true;
}

void PluginCrossover::do_set_aux_input_bufs(vector<AudioBuf*>* bufs) {

}

void PluginCrossover::finalize_buffer_init() {

}

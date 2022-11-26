/*
 * plugin_bass_enhancer.cpp
 *
 *  Created on: Nov 02, 2022
 *      Author: zaremba
 */

#include <plugin_bass_enhancer.h>

/*
 * SECOND-ORDER BUTTERWORTH SOS COEFFICIENTS FOR CROSSOVER FREQUENCY OF 60 Hz

 belo_sos =

   1.0000   2.0000   1.0000   1.0000  -1.9889   0.9890
gbelo = 1.5336e-05

behi_sos =
   1.0000  -2.0000   1.0000   1.0000  -1.9889   0.9890
gbehi = 0.9945

*/

PluginBassEnhancer::PluginBassEnhancer(const std::string& _name, uint32_t samprate, uint32_t bufsize) : ProcessorPlugin(_name.c_str(), samprate, bufsize) {
	// TODO Auto-generated constructor stub
	belo = new SOS(1.5336e-05, 1.0000,  2.0000, 1.0000, 1.0000, -1.9889, 0.9890);
	behi = new SOS(0.9945,     1.0000, -2.0000, 1.0000, 1.0000, -1.9889, 0.9890);
	cross_loL = new FilterLR4(belo, m_bufsize);
	cross_loR = new FilterLR4(belo, m_bufsize);
	cross_hiL = new FilterLR4(behi, m_bufsize);
	cross_hiR = new FilterLR4(behi, m_bufsize);

	incross_lo_outL = new float[m_bufsize];
	incross_lo_outR = new float[m_bufsize];
	incross_hi_outL = new float[m_bufsize];
	incross_hi_outR = new float[m_bufsize];

	monolo = new float[m_bufsize];
}

PluginBassEnhancer::~PluginBassEnhancer() {
	// TODO Auto-generated destructor stub
	delete[] incross_lo_outL;
	delete[] incross_lo_outR;
	delete[] incross_hi_outL;
	delete[] incross_hi_outR;
	delete[] monolo;
}

int PluginBassEnhancer::do_process() {

	if(m_enabled) {
		cross_loL->process(in_L, incross_lo_outL);
		cross_loR->process(in_R, incross_lo_outR);
		cross_hiL->process(in_L, incross_hi_outL);
		cross_hiR->process(in_R, incross_hi_outR);

/*		for(uint32_t i = 0; i < m_bufsize; i++) {
			monolo = cross_loL[i] + cross_loR[i];

			if((lastmonolo > 0.0) && (lastmonolo <= 0.0)) {
				integrate_out[i] = 0.0;
			}
			else {
				integrate_out[i] = last_integrate_out + lastmonolo;
			}

			last_integrate_out = integrate_out[i];
			lastmonolo = monolo;
		}
*/

	}
	else {
		memcpy(out_L, in_L, m_bufsize * sizeof(float));
		memcpy(out_R, in_R, m_bufsize * sizeof(float));
	}
}

fmsmoov::PluginConfigResponse PluginBassEnhancer::do_change_cfg(const fmsmoov::PluginConfig& cfg) {
	fmsmoov::PluginConfigResponse pcr;
	return pcr;
}

bool PluginBassEnhancer::do_init(const fmsmoov::PluginConfig& cfg) {

}

void PluginBassEnhancer::do_set_aux_input_bufs(vector<AudioBuf*>* bufs) {

}

void PluginBassEnhancer::finalize_buffer_init() {

}

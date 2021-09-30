/*
 * fm_smoov.cc
 *
 *	Created on: A long time ago in a much less experienced place
 *  C++ified on: Jun 5, 2020
 *      Author: zaremba
 */


#include "fm_smoov.h"

#include <agc.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <getopt.h>

#include <ncurses/ncurses.h>
#include <cdk.h>
#include <gain.h>

#include <jack/jack.h>

//#include "cmd_server.h"
#include "crossover_twoband.h"
#include "hpf_30hz.h"
#include "lpf_15khz.h"
#include "compressor.h"
#include "crossover.h"
#include "stereo_enhancer.h"
#include "fft.h"
#include "websocket_server.h"
#include "phase_rotator.h"
#include "signal_generator.h"
#include "agc_2_band.h"
#include "generic_delay.h"
#include "gain.h"

using namespace std;

//#define CAPTURE_TAP
#define MAX_ALSA_DEVICE_STRLEN 64
#define START_JACK

FMSmoov::FMSmoov()
{
	cout << "FMSmoov CTOR" << endl;

	tempaL = new float[1024];
	tempaR = new float[1024];
	tempbL = new float[1024];
	tempbR = new float[1024];
}

FMSmoov::~FMSmoov()
{
	cout << "FMSmoov DTOR" << endl;

	delete[] tempaL;
	delete[] tempaR;
	delete[] tempbL;
	delete[] tempbR;

	delete limiter_settings[0];
    delete limiter_settings[1];
	delete limiter_settings[2];
	delete limiter_settings[3];
	delete limiter_settings[4];
	delete limiter_settings[5];
	delete[] limiter_settings;

}

void FMSmoov::go()
{
	cout << "Starting..." << endl;

	master_bypass = false;
	hpf30Hz_bypass = true;
	phase_rotator_bypass = true;
	stereo_enh_bypass = true;
	ws_agc_bypass = true;
	agc2b_bypass = true;
	limiter6b_bypass = true;
	lpf15kHz_bypass = true;

	delay_line_bypass = false;


	hpf30Hz = new HPF30Hz(1024);
	phase_rotator = new PhaseRotator(1024);
	stereo_enh = new StereoEnhancer(2);

	agc_params = new AGCParams*[2];
	agc_params[0] = new AGCParams(-24.0, -44.0, .25, 8.0);
	agc_params[1] = new AGCParams(-24.0, -44.0, .25, 8.0);

	agc2b = new AGC2band(agc_params, 1024);
	//ws_agc = new AGC(-16.0, -40.0, 2.0, 4.0, "wideband_slow_agc");

	limiter_settings = new LimiterSettings*[6];
	limiter_settings[0] = new LimiterSettings(120,   -6,   3, 1, 0.080000, 0.120000); //b0 - 150 Hz
	limiter_settings[1] = new LimiterSettings(120,   -5,   2, 1, 0.030000, 0.050000); //b1 - 420 Hz
	limiter_settings[2] = new LimiterSettings(120,   -5,   2, 1, 0.001000, 0.002500); //b2 - 700 Hz
	limiter_settings[3] = new LimiterSettings(120,   -5,   2, 1, 0.000750, 0.001050); //b3 - 1.6 kHz
	limiter_settings[4] = new LimiterSettings(120,   -5,   4, 1, 0.000300, 0.000500); //b4 - 3.7 kHz
	limiter_settings[5] = new LimiterSettings(120,   -5,   2, 1, 0.000040, 0.000090); //b5 - 6.2 kHz

	limiter6b = new Limiter6band(limiter_settings, 1024);
	limiter6b->band_enable(true, true, true, true, true, true);

	lpf15kHz = new LPF15kHz(1024);

	delay_line = new DelayLine(48000, 0, 2048);

	gain_final = new Gain(8.0, 8.0, string("final gain"));

	cmd_handler = CommandHandler::get_instance();
	SUBSCRIBER me { (void*) this, this->command_handler_callback_wrapper };
	cmd_handler->add_subscriber(me);
	cmd_handler->go();

	start_jack();
	ws_server = new WebsocketServer();
	ws_server->go();

	cout << "All started. FMSmoov is on the air." << endl;
}

void FMSmoov::stop()
{
	cout << "Stopping and cleaning up..." << endl;
	ws_server->stop();
	cmd_handler->stop();
	stop_jack();

	delete hpf30Hz;
	delete phase_rotator;
	delete stereo_enh;
	//delete ws_agc;
	delete lpf15kHz;
	delete delay_line;
	delete ws_server;

	cout << "All stop." << endl;
}

void FMSmoov::start_jack()
{
	client = jack_client_open (client_name, options, &status, server_name);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}

	/* tell the JACK server to call `process()' whenever
	   there is work to be done.
	*/

	cout << "jack client is open, setting process callback..." << endl;

	jack_set_process_callback (client, jack_process_callback_wrapper, this);

	/* tell the JACK server to call `jack_shutdown()' if
	   it ever shuts down, either entirely, or if it
	   just decides to stop calling us.
	*/

	jack_on_shutdown(client, jack_shutdown_wrapper, this);

	/* display the current sample rate.
	 */

	printf ("engine sample rate: %" PRIu32 "\n",
		jack_get_sample_rate (client));

	/* create two ports */

	input_port_L = jack_port_register (client, "fmsmoov_inputL",
					 "32 bit float mono audio",
					 JackPortIsInput, 0);
	input_port_R = jack_port_register (client, "fmsmoov_inputR",
							 "32 bit float mono audio",
							 JackPortIsInput, 0);
	output_port_L = jack_port_register (client, "fmsmoov_outputL",
					  //JACK_DEFAULT_AUDIO_TYPE,
					 "32 bit float mono audio",
					  JackPortIsOutput, 0);
	output_port_R = jack_port_register (client, "fmsmoov_outputR",
							  //JACK_DEFAULT_AUDIO_TYPE,
							 "32 bit float mono audio",
							  JackPortIsOutput, 0);

	if ((input_port_L == NULL) || (output_port_L == NULL) || (input_port_R == NULL)|| (output_port_R == NULL)) {
		fprintf(stderr, "no more JACK ports available\n");
		exit (1);
	}

	/* Tell the JACK server that we are ready to roll.  Our
	 * process() callback will start running now. */

	if (jack_activate (client)) {
		fprintf (stderr, "cannot activate client");
		exit (1);
	}

	/* Connect the ports.  You can't do this before the client is
	 * activated, because we can't make connections to clients
	 * that aren't running.  Note the confusing (but necessary)
	 * orientation of the driver backend ports: playback ports are
	 * "input" to the backend, and capture ports are "output" from
	 * it.
	 */

	ports = jack_get_ports (client, NULL, NULL,
				JackPortIsPhysical|JackPortIsOutput);
	if (ports == NULL) {
		fprintf(stderr, "no physical capture ports\n");
		exit (1);
	}

	if (jack_connect (client, ports[0], jack_port_name (input_port_L))) {
		fprintf (stderr, "cannot connect input port 0\n");
	}

	if (jack_connect (client, ports[1], jack_port_name (input_port_R))) {
				fprintf (stderr, "cannot connect input port 1\n");
	}


	jack_free (ports);

	ports = jack_get_ports (client, NULL, NULL,
				JackPortIsPhysical|JackPortIsInput);
	if (ports == NULL) {
		fprintf(stderr, "no physical playback ports\n");
		exit (1);
	}

	if (jack_connect (client, jack_port_name (output_port_L), ports[0])) {
		fprintf (stderr, "cannot connect output port 0\n");
	}

	if (jack_connect (client, jack_port_name (output_port_R), ports[1])) {
				fprintf (stderr, "cannot connect output port 1\n");
	}

	jack_free (ports);
}

void FMSmoov::stop_jack()
{
	jack_client_close(client);
}

int FMSmoov::jack_process_callback_wrapper(jack_nframes_t nframes, void *arg)
{
	if(1024 != nframes)
	{
		//lazy programmer hack.
		//TODO: handle different buffer sizes!
		cout << "FATAL: Getting " << nframes << " frames from JACK but I can only do 1024."
				<< " This will get fixed in future versions. " << endl;
		return -1;
	}
	FMSmoov* fms = reinterpret_cast<FMSmoov*>(arg);
	return fms->jack_process_callback(nframes, arg);
}

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by
 * the user (e.g. using Ctrl-C on a unix-ish operating system)
 */
int FMSmoov::jack_process_callback(jack_nframes_t nframes, void *arg)
{
	jack_default_audio_sample_t *inL, *inR, *outL, *outR;

	inL = (jack_default_audio_sample_t*)jack_port_get_buffer (input_port_L, nframes);
	outL = (jack_default_audio_sample_t*)jack_port_get_buffer (output_port_L, nframes);

	inR = (jack_default_audio_sample_t*)jack_port_get_buffer (input_port_R, nframes);
	outR = (jack_default_audio_sample_t*)jack_port_get_buffer (output_port_R, nframes);

	return process(inL, inR, outL, outR, nframes);
}

int FMSmoov::process(float* inL, float* inR, float* outL, float* outR, uint32_t samps)
{
	if(true == master_bypass)
	{
		memcpy(outL, inL, samps * sizeof(float));
		memcpy(outR, inR, samps * sizeof(float));
	}
	else
	{
		memcpy(tempaL, inL, samps * sizeof(float));
		memcpy(tempaR, inR, samps * sizeof(float));

		if(false == hpf30Hz_bypass)
		{
			hpf30Hz->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		if(false == phase_rotator_bypass)
		{
			phase_rotator->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		if(false == stereo_enh_bypass)
		{
			stereo_enh->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		if(false == ws_agc_bypass)
		{
			ws_agc->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		if(false == agc2b_bypass)
		{
			agc2b->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		if(false == limiter6b_bypass)
		{
			limiter6b->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		if(false == lpf15kHz_bypass)
		{
			lpf15kHz->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		if(false == delay_line_bypass)
		{
			delay_line->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		gain_final->process(tempaL, tempaR, tempaL, tempaR, samps);

		memcpy(outL, tempaL, samps * sizeof(float));
		memcpy(outR, tempaR, samps * sizeof(float));

	//	sig_gen->process(outL, outR, samps);

	//	fft_out->process(outL, outR, fftL, fftR, samps);
	}

#ifdef CAPTURE_TAP
	fwrite(fftL, sizeof(float), samps/2, capture_tap);
#endif

	return 0;
}

void FMSmoov::jack_shutdown_wrapper(void* arg)
{
	FMSmoov* fms = reinterpret_cast<FMSmoov*>(arg);
	fms->jack_shutdown_callback(arg);
}

void FMSmoov::jack_shutdown_callback(void* arg)
{
	cout << "JACK SHUTDOWN CALLBACK:  THIS IS ABNORMAL." << endl;
	exit (1);
}

int FMSmoov::command_handler_callback_wrapper(void* arg, char* msg)
{
	FMSmoov* fms = reinterpret_cast<FMSmoov*>(arg);
	return fms->command_handler_callback(msg);
}

int FMSmoov::command_handler_callback(char* msg)
{
	cout << "FMSmoov got cmd " << msg << " from cmd handler callback." << endl;
	string s(msg);
	string delimiter = ",";
	vector<string> cmd_arr;

	size_t pos = 0;
	string token;
	while ((pos = s.find(delimiter)) != string::npos)
	{
	    token = s.substr(0, pos);
	    cmd_arr.push_back(token);
	    s.erase(0, pos + delimiter.length());
	}

	cmd_arr.push_back(s);

	try
	{
		if(cmd_arr.size() > 0)
		{
			if(!cmd_arr.at(0).compare("DELAY"))
			{
				cout << "Delay changed to " << cmd_arr.at(1) << " seconds." << endl;
				stop_jack();
				delay_line->change_delay(atof(cmd_arr.at(1).c_str()));
				start_jack();
			}
		}
	}
	catch(const exception& ex)
	{
		cout << "Caught exception " << ex.what() << " trying to process command." << endl;
	}

}



float tmpbufLlo[1024];
float tmpbufRlo[1024];
float tmpbufLlo_out[1024];
float tmpbufRlo_out[1024];
float tmpbufLhi[1024];
float tmpbufRhi[1024];
float tmpbufLhi_out[1024];
float tmpbufRhi_out[1024];

float tmpbufCrossb1L[1024];
float tmpbufCrossb1R[1024];
float tmpbufCrossb2L[1024];
float tmpbufCrossb2R[1024];
float tmpbufCrossb3L[1024];
float tmpbufCrossb3R[1024];
float tmpbufCrossb4L[1024];
float tmpbufCrossb4R[1024];

float* tmpbufCrossb1[2];
float* tmpbufCrossb2[2];
float* tmpbufCrossb3[2];
float* tmpbufCrossb4[2];



float* fftL;
float* fftR;

void ouch(int sig)
{
    printf("OUCH! - I got signal %d\n", sig);
    exit(sig);
}

void combine_bands(float** inL, float** inR, float* outL, float* outR, uint32_t samps, uint32_t numbands)
{
	for(uint32_t i = 0; i < samps; i++)
	{
		outL[i] = (inL[0][i] + inL[1][i]) * 0.5;
		outR[i] = (inR[0][i] + inR[1][i]) * 0.5;
	}
}

void combine_bands2(float** inb1, float** inb2, float** inb3, float** inb4, float* outL, float* outR, uint32_t samps)
{
	for(uint32_t i = 0; i < samps; i++)
	{
		outL[i] = (inb1[0][i] + inb2[0][i] + inb3[0][i] + inb4[0][i]) * 0.25;
		outR[i] = (inb1[1][i] + inb2[1][i] + inb3[1][i] + inb4[1][i]) * 0.25;
	}
}

int main (int argc, char *argv[])
{
	struct sigaction act;
	act.sa_handler = ouch;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGSEGV, &act, 0);

	int opt = 0;
	int c = 0;

	int this_option_optind = 0;
	int option_index = 0;
	int digit_optind = 0;
	bool gui_disabled = false;
	char alsa_device[MAX_ALSA_DEVICE_STRLEN + 1];
	memset(alsa_device, 0, MAX_ALSA_DEVICE_STRLEN + 1);

	int stdinchar;

	static const struct option long_options[] = {
		{"nogui",     no_argument, 		 0,  'n' },
		{"alsadev",	  required_argument, 0,  'd' },
		//{"append",    no_argument,       0,  0 },
		//{"delete",    required_argument, 0,  0 },
		//{"verbose",   no_argument,       0,  0 },
		//{"create",    required_argument, 0, 'c'},
		//{"file",      required_argument, 0,  0 },
		{0,           0,                 0,  0 }
	};

	srand(time(0));

	while (1) {
		this_option_optind = optind ? optind : 1;
		option_index = 0;

		//c = getopt_long(argc, argv, "abc:d:012",
		c = getopt_long(argc, argv, "nd",
				 long_options, &option_index);
		if (c == -1)
			break;

		switch (c)
		{
			/*case 0:
				printf("option %s", long_options[option_index].name);
				if (optarg)
					printf(" with arg %s", optarg);
				printf("\n");
				break;

			case '0':
			case '1':
			case '2':
				if (digit_optind != 0 && digit_optind != this_option_optind)
				  printf("digits occur in two different argv-elements.\n");
				digit_optind = this_option_optind;
				printf("option %c\n", c);
				break;
			*/
			case 'n':
				printf("GUI disabled\n");
				gui_disabled = true;
				break;

			case 'd':
				printf("Using ALSA device %s\n", optarg);
				strncpy(alsa_device, optarg, MAX_ALSA_DEVICE_STRLEN);
				break;

			/*case 'b':
				printf("option b\n");
				break;

			case 'c':
				printf("option c with value '%s'\n", optarg);
				break;

			case 'd':
				printf("option d with value '%s'\n", optarg);
				break;
			*/

			case '?':
				break;

			default:
				printf("?? getopt returned character code 0%o ??\n", c);
		}
	}

	if (optind < argc)
	{
		printf("non-option ARGV-elements: ");
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
	}



//	if(false == gui_disabled)
//	{
//		initscr();			/* Start curses mode 		  */
//		keypad(stdscr, TRUE); /* enable function and arrow keys */
//		noecho();
//		raw();
//
//		WINDOW* log_win = newwin(10, COLS, LINES-10, 0);
//		box(log_win, 0, 0);
//
//		CDKSCREEN* cdkscreen = initCDKScreen(stdscr);
//
//		int len = 0;
//		int align = LEFT;
//		chtype* cht = char2Chtype("<#HL>", &len, &align);

//		CDKSLIDER* testslider = newCDKSlider (
//		                      cdkscreen,
//		                      1,
//		                      1,
//		                      "TITLE",
//		                      "LABEL",
//							  *cht,
//		                      10,
//		                      50,
//		                      0,
//		                      100,
//		                      1,
//		                      5,
//		                      TRUE,
//		                      FALSE);
//
//		drawCDKSlider (
//				testslider,
//		                      TRUE);
//
//
//
//		refresh();			/* Print it on to the real screen */
//		refreshCDKScreen(cdkscreen);
//		wrefresh(log_win);
//		getch();			/* Wait for user input */
//		endwin();			/* End curses mode		  */
//	}

	try
	{
#ifdef CAPTURE_TAP
		capture_tap = fopen("/tmp/capture_tap.pcm", "wb");
#endif

		FMSmoov* fms = new FMSmoov();
		fms->go();

		do
		{
			stdinchar = getchar();
		} while('q' != (char)stdinchar);

		fms->stop();

#ifdef START_JACK

#endif //START_JACK

#ifdef CAPTURE_TAP
		fclose(capture_tap);
#endif

	}
	catch(const exception& ex)
	{
		cout << "Failed in main: " << ex.what() << endl;
	}

	cout << "Done.  Goodbye." << endl;

	return 0;
}

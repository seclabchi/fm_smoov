/*
 * fm_smoov.cc
 *
 *	Created on: A long time ago in a much less experienced place
 *  C++ified on: Jun 5, 2020
 *      Author: zaremba
 */


#include "fm_smoov.h"

#include <mutex>
#include <condition_variable>

#include "spdlog/sinks/stdout_color_sinks.h"

#include "common_defs.h"
#include "ProcessorMain.h"

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

//#include <ncurses/ncurses.h>
//#include <cdk.h>
#include <gain.h>

#include <jack/jack.h>

//#include "cmd_server.h"
#include "crossover_twoband.h"
#include "hpf_30hz.h"
#include "lpf_15khz.h"
#include "compressor.h"
#include "crossover.h"
#include "stereo_enhancer.h"
//#include "fft.h"
//#include "websocket_server.h"
#include "phase_rotator.h"
#include "signal_generator.h"
#include "agc_2_band.h"
#include "generic_delay.h"
#include "gain.h"

using namespace std;
using namespace spdlog;

//#define CAPTURE_TAP
#define MAX_ALSA_DEVICE_STRLEN 64
#define START_JACK

FMSmoov::FMSmoov()
{
	log = spdlog::stdout_color_mt("MAIN");
	log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
	log->set_level(spdlog::level::trace);

	LOGT("FMSMoov CTOR");

	tempaL = new float[1024];
	tempaR = new float[1024];
	tempbL = new float[1024];
	tempbR = new float[1024];
}

FMSmoov::~FMSmoov()
{
	LOGT("FMSmoov DTOR");

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
	LOGI("Starting FMSmoov...");

	bool jack_started = false;
	std::mutex mutex_startup;
	std::condition_variable cv_startup;
	std::unique_lock lk(mutex_startup);

	LOGD("Constructing ProcessorMain...");
	m_audioproc = new ProcessorMain(mutex_startup, cv_startup, jack_started);
	LOGD("Starting ProcessorMain...");
	m_thread_audioproc = new std::thread(std::ref(*m_audioproc), "1234");
	LOGD("Waiting for startup confirmation from ProcessorMain...");
	cv_startup.wait(lk, [&]{return jack_started;});

	LOGI("All started. FMSmoov is on the air.");

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

	/*
	cmd_handler = CommandHandler::get_instance();
	SUBSCRIBER me { (void*) this, this->command_handler_callback_wrapper };
	cmd_handler->add_subscriber(me);
	cmd_handler->go();
	*/


	//ws_server = new WebsocketServer();
	//ws_server->go();


}

void FMSmoov::stop()
{
	LOGI("Stopping and cleaning up...");

	LOGD("Signalling audioproc to stop...");
	m_audioproc->stop();
	LOGD("Waiting to join audioproc...");
	m_thread_audioproc->join();

	LOGD("Joined.  Continuing...");

	delete m_thread_audioproc;
	delete m_audioproc;

	//ws_server->stop();
	//cmd_handler->stop();


	delete hpf30Hz;
	delete phase_rotator;
	delete stereo_enh;
	//delete ws_agc;
	delete lpf15kHz;
	delete delay_line;
	//delete ws_server;

	LOGI("All stop.");
}

/*
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
				//stop_jack();
				delay_line->change_delay(atof(cmd_arr.at(1).c_str()));
				//start_jack();
			}
		}
	}
	catch(const exception& ex)
	{
		cout << "Caught exception " << ex.what() << " trying to process command." << endl;
	}

}

*/

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

	int stdinchar;

	static const struct option long_options[] = {
		{"nogui",     no_argument, 		 0,  'n' },
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

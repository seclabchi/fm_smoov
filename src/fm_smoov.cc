#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <getopt.h>

#include <ncurses/ncurses.h>
#include <cdk.h>

#include <jack/jack.h>

//#include "cmd_server.h"
#include "agc.h"
#include "crossover_twoband.h"
#include "hpf_30hz.h"
#include "lpf_15khz.h"
#include "compressor.h"
#include "crossover.h"
#include "stereo_enhancer.h"

#include "websocket_server.h"

using namespace std;

//#define CAPTURE_TAP

jack_port_t *input_port_L;
jack_port_t *input_port_R;
jack_port_t *output_port_L;
jack_port_t *output_port_R;
jack_client_t *client;

AGC* agc_lo, *agc_hi;
CrossoverTwoband* crossover2b;
HPF30Hz* hpf30Hz;
LPF15kHz* lpf15kHz;
Compressor* comp_b1;
Compressor* comp_b2;
Compressor* comp_b3;
Compressor* comp_b4;
Crossover* crossover4b;
StereoEnhancer* stereo_enh;
Compressor* final_lim;

WebsocketServer* ws_server;

#ifdef CAPTURE_TAP
FILE* capture_tap;
#endif

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

float* tmpbufL[2];
float* tmpbufR[2];

float* tmpbufLout[2];
float* tmpbufRout[2];

void ouch(int sig)
{
    printf("OUCH! - I got signal %d\n", sig);
    exit(sig);
}

void combine_bands(float** inL, float** inR, float* outL, float* outR, uint32_t samps, uint32_t numbands)
{
	for(uint32_t i = 0; i < samps; i++)
	{
		outL[i] = (inL[0][i] + inL[1][i]) * 0.7;
		outR[i] = (inR[0][i] + inR[1][i]) * 0.7;
	}
}

void combine_bands2(float** inb1, float** inb2, float** inb3, float** inb4, float* outL, float* outR, uint32_t samps)
{
	for(uint32_t i = 0; i < samps; i++)
	{
		outL[i] = (inb1[0][i] + inb2[0][i] + inb3[0][i] + inb4[0][i]) * 0.3;
		outR[i] = (inb1[1][i] + inb2[1][i] + inb3[1][i] + inb4[1][i]) * 0.3;
	}
}

void fmsmoov_procmain(float* inL, float* inR, float* outL, float* outR, uint32_t samps)
{
	hpf30Hz->process(inL, inR, inL, inR, samps);

	stereo_enh->process(inL, inR, outL, outR, samps);

	crossover2b->process(inL, inR, tmpbufL, tmpbufR, samps);
	agc_lo->process(tmpbufL[0], tmpbufR[0], tmpbufLout[0], tmpbufRout[0], samps);
	agc_hi->process(tmpbufL[1], tmpbufR[1], tmpbufLout[1], tmpbufRout[1], samps);

	combine_bands(tmpbufLout, tmpbufRout, outL, outR, samps, 2);

	crossover4b->process(outL, outR, tmpbufCrossb1, tmpbufCrossb2, tmpbufCrossb3, tmpbufCrossb4, samps);

	comp_b1->process(tmpbufCrossb1[0], tmpbufCrossb1[1], tmpbufCrossb1[0], tmpbufCrossb1[1], samps);
	comp_b2->process(tmpbufCrossb2[0], tmpbufCrossb2[1], tmpbufCrossb2[0], tmpbufCrossb2[1], samps);
	comp_b3->process(tmpbufCrossb3[0], tmpbufCrossb3[1], tmpbufCrossb3[0], tmpbufCrossb3[1], samps);
	comp_b4->process(tmpbufCrossb4[0], tmpbufCrossb4[1], tmpbufCrossb4[0], tmpbufCrossb4[1], samps);

	combine_bands2(tmpbufCrossb1, tmpbufCrossb2, tmpbufCrossb3, tmpbufCrossb4, outL, outR, samps);

	final_lim->process(outL, outR, outL, outR, samps);

	lpf15kHz->process(outL, outR, outL, outR, samps);

#ifdef CAPTURE_TAP
	fwrite(outL, sizeof(float), samps, capture_tap);
#endif
}

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by
 * the user (e.g. using Ctrl-C on a unix-ish operating system)
 */
int
process (jack_nframes_t nframes, void *arg)
{
	jack_default_audio_sample_t *inL, *inR, *outL, *outR;

	inL = (jack_default_audio_sample_t*)jack_port_get_buffer (input_port_L, nframes);
	outL = (jack_default_audio_sample_t*)jack_port_get_buffer (output_port_L, nframes);

	inR = (jack_default_audio_sample_t*)jack_port_get_buffer (input_port_R, nframes);
	outR = (jack_default_audio_sample_t*)jack_port_get_buffer (output_port_R, nframes);

	fmsmoov_procmain(inL, inR, outL, outR, nframes);

/*
    memcpy (outL, inL,
		sizeof (jack_default_audio_sample_t) * nframes);

	memcpy (outR, inR,
			sizeof (jack_default_audio_sample_t) * nframes);
*/
	return 0;
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void
jack_shutdown (void *arg)
{
	exit (1);
}


int main (int argc, char *argv[])
{
	struct sigaction act;
	act.sa_handler = ouch;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGSEGV, &act, 0);

	tmpbufL[0] = tmpbufLlo;
	tmpbufL[1] = tmpbufLhi;
	tmpbufR[0] = tmpbufRlo;
	tmpbufR[1] = tmpbufRhi;

	tmpbufLout[0] = tmpbufLlo_out;
	tmpbufLout[1] = tmpbufLhi_out;
	tmpbufRout[0] = tmpbufRlo_out;
	tmpbufRout[1] = tmpbufRhi_out;

	tmpbufCrossb1[0] = tmpbufCrossb1L;
	tmpbufCrossb1[1] = tmpbufCrossb1R;
	tmpbufCrossb2[0] = tmpbufCrossb2L;
	tmpbufCrossb2[1] = tmpbufCrossb2R;
	tmpbufCrossb3[0] = tmpbufCrossb3L;
	tmpbufCrossb3[1] = tmpbufCrossb3R;
	tmpbufCrossb4[0] = tmpbufCrossb4L;
	tmpbufCrossb4[1] = tmpbufCrossb4R;

	int opt = 0;
	int c = 0;

	int this_option_optind = 0;
	int option_index = 0;
	int digit_optind = 0;
	bool gui_disabled = false;

	int stdinchar;

	const char **ports;
	const char *client_name = "FMsmoov";
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	jack_status_t status;

	static struct option long_options[] = {
		{"nogui",     no_argument, 		 0,  'n' },
		//{"append",    no_argument,       0,  0 },
		//{"delete",    required_argument, 0,  0 },
		//{"verbose",   no_argument,       0,  0 },
		//{"create",    required_argument, 0, 'c'},
		//{"file",      required_argument, 0,  0 },
		{0,           0,                 0,  0 }
	};

	while (1) {
		this_option_optind = optind ? optind : 1;
		option_index = 0;

		//c = getopt_long(argc, argv, "abc:d:012",
		c = getopt_long(argc, argv, "n",
				 long_options, &option_index);
		if (c == -1)
			break;

		switch (c)
		{
			case 0:
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

	if(false == gui_disabled)
	{
		initscr();			/* Start curses mode 		  */
		keypad(stdscr, TRUE); /* enable function and arrow keys */
		noecho();
		raw();

		WINDOW* log_win = newwin(10, COLS, LINES-10, 0);
		box(log_win, 0, 0);

		CDKSCREEN* cdkscreen = initCDKScreen(stdscr);

		int len = 0;
		int align = LEFT;
		chtype* cht = char2Chtype("<#HL>", &len, &align);

		CDKSLIDER* testslider = newCDKSlider (
		                      cdkscreen,
		                      1,
		                      1,
		                      "TITLE",
		                      "LABEL",
							  *cht,
		                      10,
		                      50,
		                      0,
		                      100,
		                      1,
		                      5,
		                      TRUE,
		                      FALSE);

		drawCDKSlider (
				testslider,
		                      TRUE);



		refresh();			/* Print it on to the real screen */
		refreshCDKScreen(cdkscreen);
		wrefresh(log_win);
		getch();			/* Wait for user input */
		endwin();			/* End curses mode		  */
	}

	try
	{
#ifdef CAPTURE_TAP
		capture_tap = fopen("/tmp/capture_tap.pcm", "wb");
#endif

		crossover2b = new CrossoverTwoband(1024);
		agc_lo = new AGC(-18.0, -60.0, 0.8, 7.0, string("AGC_lo"));
		agc_hi = new AGC(-18.0, -60.0, 0.8, 7.0, string("AGC_hi"));
		hpf30Hz = new HPF30Hz(1024);
		lpf15kHz = new LPF15kHz(1024);
		crossover4b = new Crossover(1024);
		/*compressor CTOR: float _R, float _T, float _G, float _W, float _Tatt, float _Trel */
		comp_b1 = new Compressor(18, -29, -23, 0, .05, .10);
		comp_b2 = new Compressor(18, -27, -19, 0, 0.01 , 0.08);
		comp_b3 = new Compressor(18, -27, -16, 0, 0.001, 0.05);
		comp_b4 = new Compressor(18, -27, -14, 0, 0.0005, 0.005);
		stereo_enh = new StereoEnhancer(1.5);
		final_lim = new Compressor(50, -4, -10, 4, 0.0005, 0.005);

		ws_server = new WebsocketServer();
		ws_server->go();
		//CmdServer* cmd_server = new CmdServer();
		//cmd_server->start();

		/* open a client connection to the JACK server */

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

		jack_set_process_callback (client, process, 0);

		/* tell the JACK server to call `jack_shutdown()' if
		   it ever shuts down, either entirely, or if it
		   just decides to stop calling us.
		*/

		jack_on_shutdown (client, jack_shutdown, 0);

		/* display the current sample rate.
		 */

		printf ("engine sample rate: %" PRIu32 "\n",
			jack_get_sample_rate (client));

		/* create two ports */

		input_port_L = jack_port_register (client, "inputL",
						 "32 bit float mono audio",
						 JackPortIsInput, 0);
		input_port_R = jack_port_register (client, "inputR",
								 "32 bit float mono audio",
								 JackPortIsInput, 0);
		output_port_L = jack_port_register (client, "outputL",
						  //JACK_DEFAULT_AUDIO_TYPE,
						 "32 bit float mono audio",
						  JackPortIsOutput, 0);
		output_port_R = jack_port_register (client, "outputR",
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


		free (ports);

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

		free (ports);

		/* keep running until stopped by the user */

		//sleep (-1);

		/* this is never reached but if the program
		   had some other way to exit besides being killed,
		   they would be important to call.
		*/

		do
		{
			stdinchar = getchar();
		} while('q' != (char)stdinchar);

		ws_server->stop();
		jack_client_close (client);
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

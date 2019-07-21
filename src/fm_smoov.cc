#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <semaphore.h>
#include <getopt.h>

#include <ncurses/ncurses.h>
#include <cdk.h>

#include "pcm_devices.h"
#include "pcm_thread_capture.h"
#include "pcm_thread_playback.h"
#include "pcm_device_capture.h"
#include "pcm_device_playback.h"

using namespace std;

int main (int argc, char *argv[])
{
	int opt = 0;
	int c = 0;

	int this_option_optind = 0;
	int option_index = 0;
	int digit_optind = 0;
	bool gui_disabled = false;

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
		PCMDevices* devs = PCMDevices::getInstance();

		vector<PCMDevice*> playback_devices = devs->get_playback_devices();
		vector<PCMDevice*> capture_devices = devs->get_capture_devices();

		cout << "# playback devices: " << playback_devices.size() << endl;
		cout << "# capture devices: " << capture_devices.size() << endl;

		PCMDevice* dev_pb = playback_devices.at(1);
		PCMDevice* dev_cap = capture_devices.at(1);

		//start the playback thread.  It will signal when it's opened and ready for playback
		//then start the capture thread with the PB thread's buffer size, so the PB buffer can
		//be instantly filled to prevent an underrun.

		sem_t sem_cap_start;
		sem_init(&sem_cap_start, 0, 0);

		sem_t sem_pb_opened;
		sem_init(&sem_pb_opened, 0, 0);

		cout << "Playback device: " << playback_devices.at(1)->get_name() << endl;
		PCMThreadPlayback* playback_thread = new PCMThreadPlayback(dev_pb);
		playback_thread->start(&sem_cap_start, &sem_pb_opened);
		cout << "Waiting for PB to be opened..." << endl;
		sem_wait(&sem_pb_opened);
		uint32_t pb_buf_size = dev_pb->get_buffer_size();

		cout << "Capture device: " << capture_devices.at(1)->get_name() << endl;
		PCMThreadCapture* capture_thread = new PCMThreadCapture(dev_cap, pb_buf_size);
		capture_thread->start(&sem_cap_start, &sem_pb_opened);

		int stdinchar;

		do
		{
			stdinchar = getchar();
		} while('q' != (char)stdinchar);

		playback_thread->stop();
		capture_thread->stop();
	}
	catch(const exception& ex)
	{
		cout << "Failed in main: " << ex.what() << endl;
	}

	return 0;
}

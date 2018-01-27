#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Button.H>

static Fl_Text_Buffer* buff = 0;
static Fl_Text_Display *disp = 0;
static uint32_t connect_attempts = 0;

void append_log_msg(char* msg)
{
    char logmsgbuf[512];
    memset(logmsgbuf, 0, 512);
    sprintf(logmsgbuf, "%d: %s\n", clock(), msg);
    int start_pos = buff->length();
    buff->insert(start_pos, logmsgbuf);
    int numlines = buff->count_lines(0, buff->length());
    disp->scroll(numlines, 0);
}

void connect_button_pressed(Fl_Widget* w)
{
    append_log_msg("Connect button pressed.");
}

int main(int argc, char **argv) {
    Fl_Window *window = new Fl_Window(640,480, "FM SMOOV(TM) Broadcast Processor");  
    window->position(200, 100);

    buff = new Fl_Text_Buffer();
    disp = new Fl_Text_Display(10, window->h()-100, window->w()-20, 90);
    disp->buffer(buff);                 // attach text buffer to display widget

    Fl_Button* button_connect = new Fl_Button(10, 10, 80, 20, "CONNECT");
    button_connect->callback(connect_button_pressed);

    window->end();
    window->show(argc, argv);

    return Fl::run();
}
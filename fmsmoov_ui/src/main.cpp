#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <sstream>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_PNG_Image.H>

#include "Fl_Tone_Gen.h"

#include "logger.h"

using namespace std;
using namespace Tonekids;

static Fl_Text_Buffer* buff = 0;
static Fl_Text_Buffer* log_style_buf = 0;
static Fl_Text_Display *disp = 0;
static uint32_t connect_attempts = 0;
static Fl_Button* button_connect = 0;
static Fl_Check_Button* button_tonegen = 0;
static Fl_Input* input_remote_ip = 0;
static Fl_Value_Input* value_tonegen = 0;
static bool is_connected = false;
static Logger* logger = 0;

static int sockfd = 0;
static int portno = 2323;

static struct sockaddr_in serv_addr;
static struct hostent *server;

static stringstream logmsg_ss;

Fl_Text_Display::Style_Table_Entry log_style_table[] = {
         // FONT COLOR      FONT FACE   FONT SIZE
         // --------------- ----------- --------------
         {  FL_GRAY,        FL_COURIER, 12 }, // A 
         {  FL_BLACK,       FL_COURIER, 12 }, // B
         {  FL_DARK_GREEN,  FL_COURIER, 12 }, // C 
         {  FL_DARK_YELLOW, FL_COURIER, 12 }, // D
         {  FL_RED,         FL_COURIER, 12 }, // E
         {  FL_MAGENTA,     FL_COURIER, 12 }  // F
};

string log_lvl_to_style(LogLevel lvl, string msg)
{
    uint32_t len = msg.length();
    char style_char = 'A';
    
    switch(lvl)
    {
        case DEBUG_LOW:
            style_char = 'A';
            break;
        case DEBUG:
        case DEBUG_HIGH:
            style_char = 'B';
            break;
        case INFO:
            style_char = 'C';
            break;
        case WARN:
            style_char = 'D';
            break;
        case ERROR:
            style_char = 'E';
            break;
        case CRITICAL:
            style_char = 'F';
            break;
        default:
            style_char = 'B';
            break;
    }
    
    string style_str(len, style_char);
    style_str.at(style_str.length()-1) = '\n';
    return style_str;
}

void append_log_msg(LogLevel lvl, string msg)
{
    int start_pos = buff->length();
    buff->insert(start_pos, msg.c_str());
    log_style_buf->insert(start_pos, log_lvl_to_style(lvl, msg).c_str());
    int numlines = buff->count_lines(0, buff->length());
    disp->scroll(numlines, 0);
}

bool connect_to_smoovd()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        logger->log_msg(ERROR, "Error %d creating socket: %s", errno, strerror(errno));
        sockfd = 0;
        return false;
    }
        
    inet_pton(AF_INET, input_remote_ip->value(), &(serv_addr.sin_addr));
    
    //server = gethostbyname("localhost");
    
    //bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //bcopy((char *)server->h_addr, 
    //     (char *)&serv_addr.sin_addr.s_addr,
    //     server->h_length);
    serv_addr.sin_port = htons(portno);
    
    char str[256];
    inet_ntop(AF_INET, &(serv_addr.sin_addr), str, 256);
    
    logger->log_msg(INFO, "Connecting to smoovd at %s:%d...", str, portno);
    
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
    {
        logger->log_msg(ERROR, "Error %d connecting to fmsmoovd: %s", errno, strerror(errno));
        return false;
    }
    
    logger->log_msg(INFO, "Connected to fmsmoovd.");
    
    return true;
}

void disconnect_from_smoovd()
{
    if(0 != sockfd)
    {
        logger->log_msg(INFO, "Disconnecting from fmsmoovd...");
        close(sockfd);
        sockfd = 0;
        button_tonegen->value(0);
        button_tonegen->deactivate();
        logger->log_msg(INFO, "Disconnected from fmsmoovd.");
    }
    else
    {
        logger->log_msg(WARN, "Not connected to fmsmoovd.");
    }
}

string send_command_to_smoovd(string cmd)
{
    logger->log_msg(DEBUG, "Sending command \'%s\' to smoovd...", cmd.c_str());
    
    if(sockfd)
    {
        const char* c = cmd.c_str();
        int n = write(sockfd, c, strlen(c));
        logger->log_msg(DEBUG, "Wrote %d bytes to smoovd.", n);
    }
    else
    {
        logger->log_msg(ERROR, "Not connected to smoovd.");
    }
    
    return "TODO:";
}

void enable_tone_generator(uint32_t freq)
{
    char msg[1025];
    memset(msg, 0, 1025);
    
    strcpy(msg, "tg0 enable 1\r\n");
    send_command_to_smoovd(msg);
    
    sprintf(msg, "tg0 freq %d\r\n", freq);
    send_command_to_smoovd(msg);
    
    logger->log_msg(INFO, "Tone generator enabled.");
}

void disable_tone_generator()
{
    const char* msg = "tg0 enable 0\r\n";
    send_command_to_smoovd(msg);
    logger->log_msg(INFO, "Tone generator disabled.");
}

void send_tone_generator_command(bool enabled, uint32_t freq)
{
    if(false == enabled)
    {
        disable_tone_generator();
    }
    else
    {
        enable_tone_generator(freq);
    }
}

void tone_gen_callback(bool enabled, uint32_t freq)
{
    send_tone_generator_command(enabled, freq);
}

void toggle_tone_gen(Fl_Widget* w)
{
    Fl_Check_Button* b = (Fl_Check_Button*)w;
    if(1 == b->value())
    {
        enable_tone_generator(1000);
    }
    else
    {
        disable_tone_generator();
    }
}

void connect_button_pressed(Fl_Widget* w)
{
    if(false == is_connected)
    {
        if(true == connect_to_smoovd())
        {
            is_connected = true;
            button_connect->label("DISCONNECT");
            button_tonegen->activate();
        }
    }
    else
    {
        disconnect_from_smoovd();
        is_connected = false;
        button_connect->label("CONNECT");
    }
}

int main(int argc, char **argv) {
    uint32_t xoff, yoff;
    
    logmsg_ss = stringstream();
    logger = new Logger("UI_MAIN");
    logger->set_ui_logmsg_callback(append_log_msg);
    
    fl_register_images();
    
    Fl_Window *window = new Fl_Window(800,600, "FM SMOOV(TM) Broadcast Processor");  
    window->position(200, 100);
    
    xoff = 10;
    yoff = 20;
    
    Fl_Group* group_communications = new Fl_Group(xoff, yoff, window->w()-20, 75, "Communications");
    group_communications->box(FL_DOWN_FRAME);
    group_communications->align(FL_ALIGN_TOP_LEFT);
    button_connect = new Fl_Button(xoff+10, yoff+10, 160, 30, "CONNECT");
    input_remote_ip = new Fl_Input(xoff+180, yoff+10, 300, 30);
    input_remote_ip->value("127.0.0.1");
    button_connect->callback(connect_button_pressed);
    group_communications->end();
    
    xoff = 10;
    yoff = 130;
    
    Fl_Group* group_sig_gen = new Fl_Group(10, 130, window->w()-20, 100, "Signal Generators");
    group_sig_gen->clip_children(1);
    group_sig_gen->box(FL_DOWN_FRAME);
    group_sig_gen->align(FL_ALIGN_TOP_LEFT);
    button_tonegen = new Fl_Check_Button(xoff+10, yoff+10, 160, 30, "Tone Generator 0");
    button_tonegen->callback(toggle_tone_gen);
    button_tonegen->deactivate();
    group_sig_gen->end();
    
    buff = new Fl_Text_Buffer();
    disp = new Fl_Text_Display(10, window->h()-150, window->w()-20, 140);
    disp->buffer(buff);  // attach text buffer to display widget
    
    log_style_buf = new Fl_Text_Buffer();
    int log_style_buf_size = sizeof(log_style_table)/sizeof(log_style_table[0]);
    disp->highlight_data(log_style_buf, log_style_table, log_style_buf_size, 'A', 0, 0);
    
    Fl_Tone_Gen* fltg = new Fl_Tone_Gen(tone_gen_callback, 10, 300);
    fltg->set_logger(logger);
    
    window->end();
    window->show(argc, argv);

    int retval = Fl::run();
    
    delete logger;
    
    return retval;
}
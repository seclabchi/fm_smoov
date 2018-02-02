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

#include "logger.h"

using namespace std;
using namespace Tonekids;

static Fl_Text_Buffer* buff = 0;
static Fl_Text_Display *disp = 0;
static uint32_t connect_attempts = 0;
static Fl_Button* button_connect = 0;
static Fl_Check_Button* button_tonegen = 0;
static Fl_Value_Input* value_tonegen = 0;
static bool is_connected = false;
static Logger* logger = 0;

static int sockfd = 0;
static int portno = 2323;

static struct sockaddr_in serv_addr;
static struct hostent *server;

static stringstream logmsg_ss;


void append_log_msg(string msg)
{
    int start_pos = buff->length();
    buff->insert(start_pos, msg.c_str());
    int numlines = buff->count_lines(0, buff->length());
    disp->scroll(numlines, 0);
}

bool connect_to_smoovd()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        logmsg_ss.str(string());
        logmsg_ss << "Error " << errno << " creating socket: " << strerror(errno);
        logger->log_msg(logmsg_ss.str());
        sockfd = 0;
        return false;
    }
        
    server = gethostbyname("localhost");
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    
    logger->log_msg("Connecting to smoovd at localhost:2323 (localhost only for now)...");
    
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
    {
        logmsg_ss.str(string());
        logmsg_ss << "Error " << errno << " connecting to fmsmoovd: " << strerror(errno);
        logger->log_msg(logmsg_ss.str());
        return false;
    }
    
    char str[256];
    inet_ntop(AF_INET, &(serv_addr.sin_addr), str, 256);
    logger->log_msg("Connected to %s:%d.", str, portno);
    
    button_tonegen->activate();
    return true;
}

void disconnect_from_smoovd()
{
    if(0 != sockfd)
    {
        close(sockfd);
        sockfd = 0;
    }
    
    button_tonegen->value(0);
    button_tonegen->deactivate();
}

string send_command_to_smoovd(string cmd)
{
    logger->log_msg("Sending command \'%s\' to smoovd...", cmd.c_str());
    
    if(sockfd)
    {
        const char* c = cmd.c_str();
        int n = write(sockfd, c, strlen(c));
        logger->log_msg("Wrote %d bytes to smoovd.", n);
    }
    else
    {
        logger->log_msg("Not connected to smoovd.");
    }
    
    return "TODO:";
}

void enable_tone_generator()
{
    const char* msg = "tg1 enable 1\r\n";
    send_command_to_smoovd(msg);
    logger->log_msg("Tone generator enabled.");
}

void disable_tone_generator()
{
    const char* msg = "tg1 enable 0\r\n";
    send_command_to_smoovd(msg);
    logger->log_msg("Tone generator disabled.");
}

void toggle_tone_gen(Fl_Widget* w)
{
    Fl_Check_Button* b = (Fl_Check_Button*)w;
    if(1 == b->value())
    {
        enable_tone_generator();
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
        logger->log_msg("Connecting to fmsmoovd...");
        if(true == connect_to_smoovd())
        {
            is_connected = true;
            button_connect->label("DISCONNECT");
            logger->log_msg("Connected to fmsmoovd.");
        }
    }
    else
    {
        logger->log_msg("Disconnecting from fmsmoovd...");
        disconnect_from_smoovd();
        is_connected = false;
        button_connect->label("CONNECT");
        logger->log_msg("Disconnected from fmsmoovd.");
    }
}

int main(int argc, char **argv) {
    logmsg_ss = stringstream();
    logger = new Logger("UI_MAIN");
    logger->set_ui_logmsg_callback(append_log_msg);
    
    fl_register_images();
    
    Fl_Window *window = new Fl_Window(800,600, "FM SMOOV(TM) Broadcast Processor");  
    window->position(200, 100);
    
    
    Fl_Group* group_communications = new Fl_Group(10, 20, window->w()-20, 75, "Communications");
    group_communications->box(FL_DOWN_FRAME);
    group_communications->align(FL_ALIGN_TOP_LEFT);
    button_connect = new Fl_Button(group_communications->x()+10, group_communications->y()+10, 160, 30, "CONNECT");
    group_communications->add(button_connect);
    
    button_connect->callback(connect_button_pressed);
    group_communications->end();
    
    Fl_Group* group_sig_gen = new Fl_Group(10, 130, window->w()-20, 100, "Signal Generators");
    group_sig_gen->clip_children(1);
    group_sig_gen->box(FL_DOWN_FRAME);
    group_sig_gen->align(FL_ALIGN_TOP_LEFT);
    button_tonegen = new Fl_Check_Button(group_sig_gen->x()+10, group_sig_gen->y()+10, 160, 30, "Tone Generator");
    //group_sig_gen->add(button_tonegen);
    button_tonegen->callback(toggle_tone_gen);
    group_sig_gen->init_sizes();
    group_sig_gen->end();
    
    buff = new Fl_Text_Buffer();
    disp = new Fl_Text_Display(10, window->h()-150, window->w()-20, 140);
    disp->buffer(buff);  // attach text buffer to display widget
    
    window->end();
    window->show(argc, argv);

    int retval = Fl::run();
    
    delete logger;
    
    return retval;
}
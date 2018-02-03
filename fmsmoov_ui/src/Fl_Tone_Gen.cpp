#include "Fl_Tone_Gen.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <FL/Fl.H>


Fl_Tone_Gen::Fl_Tone_Gen(uint32_t x, uint32_t y) : Fl_Group(x, y, 300, 50, "Tone Generator")
{
    m_log = NULL;
    this->box(FL_DOWN_FRAME);
    this->begin();
    m_enable_button = new Fl_Light_Button(x+5, y+5, 50, 40, "ON");
    m_enable_button->selection_color(FL_GREEN);
    m_freq_input = new Input_Tone_Gen(this, x+60, y+5, 75, 40);
    m_freq_input->value("440");
    m_freq_slider = new Slider_Tone_Gen(this, x+140, y+5, 150, 40);
    m_freq_slider->callback(slider_callback, this);
    this->end();
}

Fl_Tone_Gen::~Fl_Tone_Gen()
{
    
}

void Fl_Tone_Gen::set_logger(Logger* log)
{
    m_log = log;
}

void Fl_Tone_Gen::set_freq_input(double value)
{
    char valstr[64];
    sprintf(valstr, "%0.0f", value);
    m_freq_input->value(valstr);
}

void Fl_Tone_Gen::slider_callback(Fl_Widget *w, void* arg)
{
    Fl_Tone_Gen* fltg = (Fl_Tone_Gen*)arg;
    Fl_Slider* s = (Fl_Slider*) w;
    
    fltg->set_freq_input(s->value());
}


Fl_Tone_Gen::Slider_Tone_Gen::Slider_Tone_Gen(Fl_Tone_Gen* parent, int x, int y, int w, int h) : Fl_Slider(x,y,w,h) 
{
    m_parent = parent;
    this->bounds(1, 24000);
    this->type(FL_HORIZONTAL);
    this->precision(0);
    this->step(10);
    this->value(440);
}

int Fl_Tone_Gen::Slider_Tone_Gen::handle(int event)
{
    Fl_Slider::handle(event);
    
    switch(event)
    {
        case FL_PUSH:
            m_parent->m_log->log_msg(DEBUG_LOW, "FL_PUSH");
        break;
        case FL_RELEASE:
            m_parent->m_log->log_msg(DEBUG_LOW, "FL_RELEASE");
        break;
        default:
        break;
    }
    
    return 1;
}

Fl_Tone_Gen::Input_Tone_Gen::Input_Tone_Gen(Fl_Tone_Gen* parent, int x, int y, int w, int h) : Fl_Int_Input(x,y,w,h) 
{
    m_parent = parent;
}

int Fl_Tone_Gen::Input_Tone_Gen::handle(int event)
{
    Fl_Int_Input::handle(event);
    
    switch(event)
    {
        case FL_KEYDOWN:
            if(FL_Enter == Fl::event_key())
            {
                m_parent->m_log->log_msg(DEBUG_LOW, "enter key pressed");
            }
        break;
        default:
        break;
    }
    
    return 1;
}
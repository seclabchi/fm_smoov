#ifndef __FL_TONE_GEN_H__
#define __FL_TONE_GEN_H__

#include <stdint.h>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Slider.H>

#include "logger.h"

using namespace Tonekids;



class Fl_Tone_Gen : public Fl_Group
{
public:
    Fl_Tone_Gen(uint32_t x, uint32_t y);
    void set_logger(Logger* log);
    virtual ~Fl_Tone_Gen();
    static void slider_callback(Fl_Widget *w, void* arg);
    void set_freq_input(double value);
    
    class Slider_Tone_Gen : public Fl_Slider
    {
        public:
            Slider_Tone_Gen(Fl_Tone_Gen* parent, int x, int y, int w, int h);
            int handle(int event);
        private:
            Fl_Tone_Gen* m_parent;
    };
    
    class Input_Tone_Gen : public Fl_Int_Input
    {
        public:
            Input_Tone_Gen(Fl_Tone_Gen* parent, int x, int y, int w, int h);
            int handle(int event);
        private:
            Fl_Tone_Gen* m_parent;
    };
private:
    
    Fl_Light_Button* m_enable_button;
    Input_Tone_Gen* m_freq_input;
    Slider_Tone_Gen* m_freq_slider;
    Logger* m_log;
};

#endif //__FL_TONE_GEN_H__
#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "audio_params.h"

using namespace std;

class Processor
{
public:
    Processor(audio_params_t* params, string name);
    virtual ~Processor();
    virtual void process(float* buf, size_t frames) = 0;
    virtual string get_name();
    virtual string do_command(vector<string> cmds) = 0;
protected:
    Processor() {};
    string m_name;
    audio_params_t* m_params;
    float* m_buf_in;
    float* m_buf_out;
};

#endif
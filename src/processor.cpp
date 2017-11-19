#include "processor.h"

#include <string.h>

Processor::Processor(audio_params_t* params, string name)
{
    m_params = new audio_params_t();
    memcpy(m_params, params, sizeof(audio_params_t));
    m_name = name;
}

Processor::~Processor()
{
    delete m_params;
}

string Processor::get_name()
{
    return m_name;
}
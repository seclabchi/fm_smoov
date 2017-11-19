#include "processor.h"

#include <string.h>

Processor::Processor(audio_params_t* params)
{
    m_params = new audio_params_t();
    memcpy(m_params, params, sizeof(audio_params_t));
}

Processor::~Processor()
{
    delete m_params;
}
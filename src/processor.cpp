#include "processor.h"

#include <string.h>

Processor::Processor(audio_params_t* params, string name)
{
    m_params = new audio_params_t();
    memcpy(m_params, params, sizeof(audio_params_t));
    m_name = name;
    m_enabled = false;
}

Processor::~Processor()
{
    delete m_params;
}

string Processor::get_name()
{
    return m_name;
}

void Processor::enable(bool enable)
{
    m_enabled = enable;
}

string Processor::do_command(vector<string> cmds)
{
    string retval = "ERROR";
    
    if(0 == cmds.at(1).compare("enable"))
    {
        istringstream(cmds.at(2)) >> m_enabled;
        retval = "OK";
    }
    
    return retval;
}
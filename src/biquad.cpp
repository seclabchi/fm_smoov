#include "biquad.h"

#include <cstring>

BiQuad::BiQuad(float b0, float b1, float b2, float a1, uint32_t buf_size) : m_b0(b0), m_b1(b1), m_b2(b2), m_a1(a1), m_buf_size(buf_size)
{
    m_buf_out = new float[m_buf_size];
}

BiQuad::~BiQuad()
{
    delete[] m_buf_out;
}
    
    
void BiQuad::process(float* buf)
{
    for(uint32_t i = 0; i < m_buf_size; i++)
    {
        m_buf_out[i] = m_b0 * buf[i] + m_state[1];
        m_state[0] = m_b1 * buf[i] - m_a0 * m_buf_out[i] + m_state[1];
        m_state[1] = m_b2 * buf[i] - m_a1 * m_buf_out[i];
    }
    
    memcpy(buf, m_buf_out, sizeof(float)*m_buf_size);
}
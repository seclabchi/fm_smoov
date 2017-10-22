#ifndef __BIQUAD_H__
#define __BIQUAD_H__

#include <stdint.h>

class BiQuad
{
public:
    BiQuad(float b0, float b1, float b2, float a0, float a1, uint32_t buf_size);
    virtual ~BiQuad();
    void process(float* buf);
private:
    BiQuad();
    float m_b0;
    float m_b1;
    float m_b2;
    float m_a0;
    float m_a1;
    float m_state[2];
    uint32_t m_buf_size;
    float* m_buf_out;
};

#endif
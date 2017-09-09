#ifndef __PCM_TRANSFER_INTERFACE_H__
#define __PCM_TRANSFER_INTERFACE_H__

#include <stdint.h>

class PCM_Transfer_Interface
{
public:
    PCM_Transfer_Interface() {}
    virtual ~PCM_Transfer_Interface() {}
    virtual void write_buffer(uint8_t* bufsrc, uint32_t size_bytes) = 0;
    virtual void read_buffer(uint8_t** bufdst, uint32_t size_bytes) = 0;
    virtual uint32_t get_buffer_size_bytes() = 0;
    virtual uint8_t get_frame_size_bytes() = 0;
    virtual uint8_t get_buffer_size_frames() = 0;
    virtual uint8_t get_channels() = 0;
};

#endif
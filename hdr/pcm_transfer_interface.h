#ifndef __PCM_TRANSFER_INTERFACE_H__
#define __PCM_TRANSFER_INTERFACE_H__

#include <stdint.h>

class PCM_Transfer_Interface
{
public:
    PCM_Transfer_Interface() {}
    virtual ~PCM_Transfer_Interface() {}
    virtual void write_buffer(const void* bufsrc, size_t size, size_t count) = 0;
    virtual void read_buffer(void** bufdst, size_t size, size_t count) = 0;
    virtual uint32_t get_buffer_size_bytes() = 0;
    virtual uint8_t get_frame_size_bytes() = 0;
    virtual uint8_t get_buffer_size_frames() = 0;
    virtual uint8_t get_channels() = 0;
};

#endif
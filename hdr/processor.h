#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <stddef.h>
#include <stdint.h>

class Processor
{
public:
    Processor() {};
    virtual ~Processor() {};
    virtual void process(void** buf, size_t size, size_t count) = 0;
};

#endif
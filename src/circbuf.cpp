#include "circbuf.h"

#include <string.h>

Circbuf::Circbuf(uint32_t _bufsize)
{
	thebuf = new float[_bufsize];
	bufsize = _bufsize;
}

Circbuf::~Circbuf()
{
	delete[] thebuf;
}

void Circbuf::write(float* write_ptr, uint32_t len)
{
	if(len > bufremain_w)
	{
		memcpy(thebuf, write_ptr, bufremain_w);
		wptr = thebuf;
		memcpy(thebuf, write_ptr + bufremain_w, len - bufremain_w);
		bufremain_w = bufsize - (len - bufremain_w);
		wptr += len-bufremain_w;
	}
	else
	{
		memcpy(thebuf, write_ptr, len);
		bufremain_w -= len;
		wptr += len;
	}
}

void Circbuf::read(float* read_ptr, uint32_t len)
{
	if(len > bufremain_r)
	{
		memcpy(read_ptr, rptr, bufremain_r);
		rptr = thebuf;
		memcpy(read_ptr + bufremain_r, rptr, len - bufremain_r);
		bufremain_r = bufsize - (len - bufremain_r);
		rptr += len-bufremain_r;
	}
	else
	{
		memcpy(read_ptr, rptr, len);
		bufremain_r -= len;
		rptr += len;
	}
}

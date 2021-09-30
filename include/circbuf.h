#ifndef __CIRCBUF_H__
#define __CIRCBUF_H__

#include <stdint.h>

class Circbuf
{
public:
	Circbuf(uint32_t _bufsize);
	~Circbuf();
	void write(float* _wdata, uint32_t _size);
	void read(float* _rdata, uint32_t _size);
	void readoff(float* _rdata, uint32_t startoff, uint32_t _size);
private:
	float* thebuf;
	uint32_t bufsize;
	float* wptr;
	float* rptr;
	uint32_t bufremain_r, bufremain_w;

};

#endif

/*
 * audiobuf.h
 *
 *  Created on: Jun 14, 2022
 *      Author: zaremba
 */

#ifndef AUDIOBUF_H_
#define AUDIOBUF_H_

#include <stdint.h>
#include <string>

#include "spdlog/spdlog.h"

class AudioBuf {
public:
	AudioBuf(uint32_t size, std::string name);
	virtual ~AudioBuf();
	void getcopy(float* outptr, uint32_t size);
	void set(float* inptr, uint32_t size);
	float* buf();  //yeah completely violates encapsulation but we need speed?
	uint32_t size();
private:
	std::shared_ptr<spdlog::logger> log;
	float* m_buf;
	uint32_t m_size;
};

#endif /* AUDIOBUF_H_ */

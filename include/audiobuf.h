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

class AudioBuf {
public:
	typedef enum {
		ALLOCATED = 0,
		REFERENCE = 1
	} AUDIO_BUF_TYPE;

public:
	AudioBuf(std::string name, AUDIO_BUF_TYPE type, uint32_t size = 0, float* srcptr = NULL);
	AudioBuf(const AudioBuf& rhs);
	AudioBuf& operator = (const AudioBuf& rhs);
	virtual ~AudioBuf();
	bool setptr(float* inptr, uint32_t size);
	void get(float** p);  //yeah completely violates encapsulation but we need speed?
	float* get();
	const uint32_t size();
	const std::string name();
	const AUDIO_BUF_TYPE type();
private:
	float* m_buf;
	uint32_t m_size;
	std::string m_name;
	AUDIO_BUF_TYPE m_type;
};

#endif /* AUDIOBUF_H_ */

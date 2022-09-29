/*
 * audiobuf.cpp
 *
 *  Created on: Jun 14, 2022
 *      Author: zaremba
 */

#include "common_defs.h"
#include "audiobuf.h"

#include <string.h>

AudioBuf::AudioBuf(std::string name, AUDIO_BUF_TYPE type, uint32_t size, float* srcptr) : m_name(name),
												m_size(size), m_type(type) {
	if(m_type == AUDIO_BUF_TYPE::ALLOCATED) {
		m_buf = new float[m_size];
	}
	else if(m_type == AUDIO_BUF_TYPE::REFERENCE) {
		m_buf = srcptr;
	}
}

AudioBuf::~AudioBuf() {
	if(m_type == AUDIO_BUF_TYPE::ALLOCATED) {
		delete[] m_buf;
	}
	m_buf = NULL;
}

AudioBuf::AudioBuf(const AudioBuf& rhs) {
	m_name = rhs.m_name;
	m_type = rhs.m_type;
	m_size = rhs.m_size;
	if(m_type == AUDIO_BUF_TYPE::ALLOCATED) {
		m_buf = new float[rhs.m_size];
		memcpy(m_buf, rhs.m_buf, m_size * sizeof(float));
	}
	else if(m_type == AUDIO_BUF_TYPE::REFERENCE) {
		m_buf = rhs.m_buf;
	}
}

AudioBuf& AudioBuf::operator = (const AudioBuf& rhs) {
	if(this == &rhs) {
		return *this;
	}

	AudioBuf copy(rhs);
	std::swap(*this, copy);

	return *this;
}


//TODO:  if this is a reference buffer,
//this pointer/buffer will not be delete upon destruction of this object
bool AudioBuf::setptr(float* inptr, uint32_t size) {
	if(size != m_size) {
		return false;
	}

	if(AUDIO_BUF_TYPE::ALLOCATED == m_type) {
		memcpy(m_buf, inptr, m_size * sizeof(float));
	}
	else if(AUDIO_BUF_TYPE::REFERENCE == m_type) {
		m_buf = inptr;
	}

	return true;
}

void AudioBuf::get(float** p) {
	*p = m_buf;
}

float* AudioBuf::get() {
	return m_buf;
}

const uint32_t AudioBuf::size() {
	return m_size;
}

const std::string AudioBuf::name() {
	return std::string(m_name);
}

const AudioBuf::AUDIO_BUF_TYPE AudioBuf::type() {
	return m_type;
}

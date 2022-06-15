/*
 * audiobuf.cpp
 *
 *  Created on: Jun 14, 2022
 *      Author: zaremba
 */

#include "common_defs.h"
#include "audiobuf.h"

#include "spdlog/sinks/stdout_color_sinks.h"

AudioBuf::AudioBuf(uint32_t size, std::string name) {
	log = spdlog::stdout_color_mt("PLUGIN");
	log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
	log->set_level(spdlog::level::trace);
	m_buf = new float[size];
}

AudioBuf::~AudioBuf() {
	delete[] m_buf;
}

void AudioBuf::getcopy(float* outptr, uint32_t size) {
	if(size != m_size) {
		LOGE("Can't get buf of size {} with buf of size {}", m_size, size);
		return;
	}
	memcpy(outptr, m_buf, m_size * sizeof(float));
}

void AudioBuf::set(float* inptr, uint32_t size) {
	if(size != m_size) {
		LOGE("Can't set buf of size {} with buf of size {}", m_size, size);
		return;
	}
	memcpy(m_buf, inptr, m_size * sizeof(float));
}

float* AudioBuf::buf() {
	//yeah completely violates encapsulation but we need speed?
	return m_buf;
}

uint32_t AudioBuf::size() {
	return m_size;
}

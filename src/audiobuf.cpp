/*
 * audiobuf.cpp
 *
 *  Created on: Jun 14, 2022
 *      Author: zaremba
 */

#include "common_defs.h"
#include "audiobuf.h"

#include <sstream>

#include "spdlog/sinks/stdout_color_sinks.h"

AudioBuf::AudioBuf(std::string name, AUDIO_BUF_TYPE type, uint32_t size, float* srcptr) : m_name(name),
												m_size(size), m_type(type) {
	std::stringstream bufname;
	bufname << "AUDIOBUF_" << m_name;

	auto existing_log = spdlog::get(bufname.str());

	if(nullptr == existing_log) {
		log = spdlog::stdout_color_mt(bufname.str());
		log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
		log->set_level(spdlog::level::trace);
	}
	else {
		log = existing_log;
		LOGW("A logger with the name {} already exists.  Using existing.", bufname.str());
	}

	if(m_type == AUDIO_BUF_TYPE::ALLOCATED) {
		m_buf = new float[m_size];
	}
	else if(m_type == AUDIO_BUF_TYPE::REFERENCE) {
		if(NULL == srcptr) {
			LOGW("Attempting to allocate AudioBuf named {} as REFERENCE with no/NULL srcptr", m_name);
		}

		m_buf = srcptr;
	}
	LOGI("Created new AudioBuf with name {} and bufsize {}", m_name, size);
}

AudioBuf::~AudioBuf() {
	LOGD("Deleting AudioBuf with name {}...", m_name);

	if(m_type == AUDIO_BUF_TYPE::ALLOCATED) {
		delete[] m_buf;
	}
	log.reset();
	m_buf = NULL;
}

AudioBuf::AudioBuf(const AudioBuf& rhs) {
	log = rhs.log;
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

bool AudioBuf::set(float* inptr, uint32_t size) {
	if(size != m_size) {
		LOGE("Attempting to set buffer of size {} with buffer of size {}", m_size, size);
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

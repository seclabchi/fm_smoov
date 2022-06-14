#pragma once

/* FMSmoov Common Defs */

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LOGGER_TRACE

#define EXPAND_ARGS(...) __VA_ARGS__

#define LOGT(...) SPDLOG_LOGGER_TRACE(log, __VA_ARGS__)
#define LOGD(...) SPDLOG_LOGGER_DEBUG(log, __VA_ARGS__)
#define LOGI(...) SPDLOG_LOGGER_INFO(log, __VA_ARGS__)
#define LOGW(...) SPDLOG_LOGGER_WARN(log, __VA_ARGS__)
#define LOGE(...) SPDLOG_LOGGER_ERROR(log, __VA_ARGS__)
#define LOGC(...) SPDLOG_LOGGER_CRITICAL(log, __VA_ARGS__)

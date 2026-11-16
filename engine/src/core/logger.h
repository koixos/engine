#pragma once

#include "defines.h"

//controllable log levels

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

//disable debug and trace logging for release builds
#if KRELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

typedef enum log_level {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5,
} log_level;

/**
 * @brief initializes logging sys - call twice: once with st = 0 to get required mem size,
 * then a second time passing allocated mem to state
 * @param mem_requirement A ptr to hold the required mem size of internal state
 * @param st 0 if just requesting mem requirement, otherwise allocated block of memory
 */
b8 init_logging(u64 *mem_requirement, void *st);

void shutdown_logging(void *st);

KAPI void log_output(log_level lev, const char *msg, ...);

#define KFATAL(msg, ...) log_output(LOG_LEVEL_FATAL, msg, ##__VA_ARGS__);

#ifndef KERROR
#define KERROR(msg, ...) log_output(LOG_LEVEL_ERROR, msg, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
#define KWARN(msg, ...) log_output(LOG_LEVEL_WARN, msg, ##__VA_ARGS__)
#else
#define KWARN(msg, ...)
#endif

#if LOG_INFO_ENABLED == 1
#define KINFO(msg, ...) log_output(LOG_LEVEL_INFO, msg, ##__VA_ARGS__)
#else
#define KINFO(msg, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
#define KDEBUG(msg, ...) log_output(LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)
#else
#define KDEBUG(msg, ...)
#endif

#if LOG_TRACE_ENABLED == 1
#define KTRACE(msg, ...) log_output(LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)
#else
#define KTRACE(msg, ...)
#endif
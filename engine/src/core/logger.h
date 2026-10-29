#pragma once

#include "defines.h"

//define which log levels are controllable
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

b8 init_logging();
void shutdown_logging();

KAPI void log_output(log_level lev, const char *msg, ...);

//logging messages

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
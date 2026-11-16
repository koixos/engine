#include "logger.h"
#include "asserts.h"

#include "platform/platform.h"

//TODO: temporary
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//TODO: dummy st
typedef struct logger_sys_state {
    b8 inited;
} logger_sys_state;

static logger_sys_state *state_ptr;

b8 init_logging(u64 *mem_requirement, void *st) {
    //TODO: create a log file
    //TODO: this func always returns true??
    
    *mem_requirement = sizeof(logger_sys_state);

    if (st == 0)
        return true;
  
    state_ptr = st;
    state_ptr->inited = true;

    return true;
}

void shutdown_logging(void *st) {
    //TODO: cleanup logging/write queued entries 

    state_ptr = 0;
}

void log_output(log_level lev, const char *msg, ...) {
    const char *lev_strs[6] = {
        "[FATAL]: ",
        "[ERROR]: ",
        "[WARN]: ",
        "[INFO]: ",
        "[DEBUG]: ",
        "[TRACE]: ",
    };
    b8 is_error = lev < LOG_LEVEL_WARN;

    i32 buf_size = 32000;
    char *buf = malloc(buf_size * sizeof(char));

    if (buf == NULL) {
        printf("Memory allocation for logger failed\n");
        return;
    }

    //format original msg
    b8 realloced = false;
    while (1) {
        __builtin_va_list arg_ptr;
        va_start(arg_ptr, msg);
        b32 written = vsnprintf(buf, buf_size, msg, arg_ptr);
        va_end(arg_ptr);

        //if buffer already realloced
        if (realloced || written < buf_size)
            break;

        buf_size = written + 1;
        char *temp_buf = realloc(buf, buf_size * sizeof(char));
        if (temp_buf == NULL) {
            printf("Memory allocation for logger failed\n");
            free(buf);
            return;
        } 

        buf = temp_buf;
        realloced = true;
    }
    
    //trying not to use any vla... thus hard-coding the msg length
    //b32 msg_size = strlen(buf) + strlen(lev_strs[lev]) + 1;
    char out_msg[32000];

    sprintf(out_msg, "%s%s\n", lev_strs[lev], buf);

    //platform-specific output
    if (is_error)
        platform_console_write_error(out_msg, lev);
    else
        platform_console_write(out_msg, lev);

    free(buf);
}

void report_assertion_failure(const char *expr, const char *msg, const char *file, i32 line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expr, msg, file, line);
}
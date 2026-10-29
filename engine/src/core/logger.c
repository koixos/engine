#include "logger.h"
#include "asserts.h"

//TODO: temporary
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

b8 init_logging() {
    //TODO: create a log file
    return TRUE;
}

void shutdown_logging() {
    //TODO: cleanup logging/write queued entries 
}

void log_output(log_level lev, const char* msg, ...) {
    const char* lev_strs[6] = {
        "[FATAL]: ",
        "[ERROR]: ",
        "[WARN]: ",
        "[INFO]: ",
        "[DEBUG]: ",
        "[TRACE]: ",
    };

    char *buf;
    b32 buf_size = 16;
    
    buf = (char*)malloc(buf_size * sizeof(char));
    if (buf == NULL) {
        printf("Memory allocation for logger failed\n");
        return;
    }

    //format original msg
    b8 realloced = FALSE;
    while (1) {
        __builtin_va_list arg_ptr;
        va_start(arg_ptr, msg);
        b32 written = vsnprintf(buf, buf_size, msg, arg_ptr);
        va_end(arg_ptr);

        //if buffer already realloced
        if (realloced)
            break;

        buf_size = written + 1;
        char *temp_buf = realloc(buf, buf_size * sizeof(char));
        if (temp_buf == NULL) {
            printf("Memory allocation for logger failed\n");
            free(buf);
            return;
        } 

        buf = temp_buf;
        realloced = TRUE;
    }
    
    b32 msg_size = strlen(buf) + strlen(lev_strs[lev]) + 1;
    char out_msg[msg_size];

    sprintf(out_msg, "%s%s\n", lev_strs[lev], buf);

    //TODO: platform-specific output
    printf("%s", out_msg);

    free(buf);
}

void report_assertion_failure(const char* expr, const char* msg, const char* file, i32 line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expr, msg, file, line);
}
#include "logger.h"
#include "asserts.h"
#include "kstring.h"
#include "kmemory.h"

#include "platform/platform.h"
#include "platform/filesystem.h"

//TODO: temporary
#include <stdarg.h>

//TODO: this is dummy st
typedef struct logger_sys_state {
    file_handle logfile_handle;
} logger_sys_state;

static logger_sys_state *state_ptr;

void append2logfile(const char *msg) {
    if (state_ptr && state_ptr->logfile_handle.is_valid) {
        //since msg already contains a '\n', just write the bytes directly
        u64 len = str_len(msg);
        u64 written = 0;
        if (!filesystem_write(&state_ptr->logfile_handle, len, msg, &written)) 
            platform_console_write_error("Error when writing to console.log.", LOG_LEVEL_ERROR);
    }
}

b8 init_logging_sys(u64 *mem_requirement, void *st) {
    //TODO: this func always returns true??
    *mem_requirement = sizeof(logger_sys_state);
    
    if (st == 0)
    return true;
    
    state_ptr = st;

    if (!filesystem_open("console.log", FILE_MODE_WRITE, false, &state_ptr->logfile_handle)) {
        platform_console_write_error("ERROR: Unable to open console.log for writing.", LOG_LEVEL_ERROR);
        return false;
    }

    KINFO("Logging subsystem initialized.");
    return true;
}

void shutdown_logging(void *st) {
    //TODO: cleanup logging/write queued entries 
    state_ptr = 0;
}

void log_output(log_level lev, const char *msg, ...) {
    //TODO: str ops are all pretty slow, thus it needs to be
    // moved to another thread eventually, along w/ the file writes
    const char *lev_strs[6] = {
        "[FATAL]: ",
        "[ERROR]: ",
        "[WARN]: ",
        "[INFO]: ",
        "[DEBUG]: ",
        "[TRACE]: ",
    };
    b8 is_error = lev < LOG_LEVEL_WARN;

    //TODO: well u shouldnt be doing this but instead of using malloc this better...
    char out_msg[32000];
    kzero_mem(out_msg, sizeof(out_msg));

    //format original msg
    b8 realloced = false;
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, msg);
    string_format_v(out_msg, msg, arg_ptr);
    va_end(arg_ptr);

    string_format(out_msg, "%s%s\n", lev_strs[lev], out_msg);

    if (is_error)
        platform_console_write_error(out_msg, lev);
    else
        platform_console_write(out_msg, lev);

    //queue a copy to be written to the log file
    append2logfile(out_msg);
}

void report_assertion_failure(const char *expr, const char *msg, const char *file, i32 line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expr, msg, file, line);
}
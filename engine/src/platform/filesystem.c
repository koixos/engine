#include "filesystem.h"

#include "core/logger.h"
#include "core/kmemory.h"
#include "core/kstring.h"

//TODO: to be removed
#include <stdio.h>
#include <sys/stat.h>

b8 filesystem_exists(const char *path) {
    struct stat buf;
    return stat(path, &buf) == 0;
}

b8 filesystem_open(const char *path, file_modes mode, b8 bin, file_handle *out_handle) {
    out_handle->is_valid = false;
    out_handle->handle = 0;

    const char *mode_str;
    if ((mode & FILE_MODE_READ) != 0 && (mode & FILE_MODE_WRITE) != 0) {
        mode_str = bin ? "w+b" : "w+";
    } else if ((mode & FILE_MODE_READ) != 0 && (mode & FILE_MODE_WRITE) == 0) {
        mode_str = bin ? "rb" : "r";
    } else if ((mode & FILE_MODE_READ) == 0 && (mode & FILE_MODE_WRITE) != 0) {
        mode_str = bin ? "wb" : "w";
    } else {
        KERROR("Invalid mode passed while trying to open file: '%s'", path);
        return false;
    }

    FILE *fp = fopen(path, mode_str);
    if (!fp) {
        KERROR("Error opening file: '%s'", path);
        return false;
    }

    out_handle->handle = fp;
    out_handle->is_valid = true;
    return true;
}

void filesystem_close(file_handle *handle) {
    if (handle->handle) {
        fclose((FILE *)handle->handle);
        handle->handle = 0;
        handle->is_valid = false;
    }
}

b8 filesystem_read_line(file_handle *handle, char **line_buf) {
    if (handle->handle) {
        //assuming enough space for a single line
        char buf[32000];
        if (fgets(buf, 32000, (FILE *)handle->handle)) {
            //u64 len = str_len(buf);
            //*line_buf = kallocate((sizeof(char) * len) + 1, MEM_TAG_STRING); //str_dup already allocates?
            *line_buf = str_dup(buf);
            return true;
        }
    }
    return false;
}

b8 filesystem_read(file_handle *handle, u64 data_size, void *out_data, u64 *out_bytes_read) {
    if (handle->handle && out_data) {
        *out_bytes_read = fread(out_data, 1, data_size, (FILE *)handle->handle);
        if (*out_bytes_read != data_size)
            return false;
        return true;
    }
    return false;
}

b8 filesystem_read_all_bytes(file_handle *handle, u8 **out_bytes, u64 *out_bytes_read) {
    if (handle->handle) {
        //file size
        fseek((FILE *)handle->handle, 0, SEEK_END);
        u64 size = ftell((FILE *)handle->handle);
        rewind((FILE *)handle->handle);
        
        *out_bytes = kallocate(sizeof(u8) * size, MEM_TAG_STRING);
        *out_bytes_read = fread(*out_bytes, 1, size, (FILE *)handle->handle);
        if (*out_bytes_read != size)
            return false;
        return true;
    }
    return false;
}

b8 filesystem_write_line(file_handle *handle, const char *text) {
    if (handle->handle) {
        i32 res = fputs(text, (FILE *)handle->handle);
        if (res == EOF)
            return false;

        res = fputc('\n', (FILE *)handle->handle);

        //make sure to flush the stream to write to the file immediately,
        // so the data loss in an event of a crash is prevented
        fflush((FILE *)handle->handle);
        return true;
    }
    return false;
}

b8 filesystem_write(file_handle *handle, u64 data_size, const void *data, u64 *out_bytes_written) {
    if (handle->handle) {
        *out_bytes_written = fwrite(data, 1, data_size, (FILE *)handle->handle);
        if (*out_bytes_written != data_size)
            return false;
        fflush((FILE *)handle->handle);
        return true;
    }
    return false;
}

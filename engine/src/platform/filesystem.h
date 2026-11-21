#pragma once

#include "defines.h"

/**
 * Holds a handle to a file
 */
typedef struct file_handle {
    /**
     * Opaque handle to an internal file handle
     */
    void *handle;
    b8 is_valid;
} file_handle;

typedef enum file_modes {
    FILE_MODE_READ = 0x1,
    FILE_MODE_WRITE = 0x2
} file_modes;

/**
 * Checks if a file with the given path exists
 */
b8 filesystem_exists(const char *path);

/**
 * Attempt to open the file located at given path
 * @param path The path of the file to be opened
 * @param mode Mode flags for the file when opened (see file_modes enum in filesystem.h)
 * @param bin Indicates if the file should be opened in binary mode
 * @param out_handle A ptr to a file_handle structure which holds the handle info
 * @returns true if opened successfully; otherwise false
 */
b8 filesystem_open(const char *path, file_modes mode, b8 bin, file_handle *out_handle);

/**
 * Closes the file with the given handle
 */
void filesystem_close(file_handle *handle);

/**
 * Reads up to a newline or EOF; allocates *line_buf, which must be freed by the caller
 * @param handle A ptr to a file_handle structure
 * @param line_buf A ptr to a char array which will be allocated and populated by this method
 * @returns true if successful; otherwise false
 */
b8 filesystem_read_line(file_handle *handle, char **line_buf);

/**
 * Reads up to data_size bytes of data into out_bytes_read, and allocates out_data which
 *  must be freed by the caller
 * @param handle A ptr to a file_handle structure
 * @param data_size # of bytes to be read
 * @param out_data A ptr to a block of mem to be populated by this method
 * @param out_bytes_read A ptr to a number which will be populated with the # of bytes
 * actually read from the file
 * @returns true if opened successfully; otherwise false
 */
b8 filesystem_read(file_handle *handle, u64 data_size, void *out_data, u64 *out_bytes_read);

/**
 * Reads all bytes in the file into out_bytes_read, and allocates out_bytes which
 * must be freed by the caller
 * @param handle A ptr to a file_handle structure
 * @param out_bytes A ptr to a byte array which will be allocated and populated by this method
 * @param out_bytes_read A ptr to a number which will be populated with the # of bytes
 * actually read from the file
 * @returns true if successful; otherwise false
 */
b8 filesystem_read_all_bytes(file_handle *handle, u8 **out_bytes, u64 *out_bytes_read);

/**
 * Writes text to the given file, appending a '\n' afterward
 */
b8 filesystem_write_line(file_handle *handle, const char *text);

/**
 * Writes to the given file
 * @param handle A ptr to a file_handle structure
 * @param data_size The size of the data to be written in bytes
 * @param data The data to be written
 * @param out_bytes_written A ptr to a number which will be populated with the # of bytes
 * actually written to the file
 * @returns true if opened successfully; otherwise false
 */
b8 filesystem_write(file_handle *handle, u64 data_size, const void *data, u64 *out_bytes_written);

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>

#define FILESYSTEM_MAX_OPEN_FILES 4U

/**
 * A function that will be called when a filesystem error occurs.  Will receive a string with details about the error.
 */
typedef void(*FilesystemErrorFunc)(const char*);

/**
 * Initializes the filesystem.  This method should be called before any other filesystem methods.
 * @param filesystem_path_prefix prefix to be prepended to all calls to filesystem_open.  This path should not end with `/`.
 */
void filesystem_init(const char* filesystem_path_prefix);
void filesystem_uninit();
int filesystem_open(const char* path);
void filesystem_close(uint32_t handle);
int filesystem_read(void* buf, int size, int count, int handle);

/**
 * Gets the asset directory base path.  This can be useful if you need an explicit absolute path.  Caller should not free or modify this path.
 * @return Asset directory base path or NULL on systems where there is no base directory, such as N64.
 */
const char* filesystem_get_asset_base_path();

/**
 * Seek the underlying source.  Use SEEK_SET, SEEK_CUR, SEEK_END.
 */
void filesystem_seek(int handle, int offset, int origin);

/** Sets a function to be called when a filesystem error occurs. */
void filesystem_set_error_callback(FilesystemErrorFunc func);

#endif
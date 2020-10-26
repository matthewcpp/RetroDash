#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>

#define FILESYSTEM_MAX_OPEN_FILES 4U

/**
 * Initializes the filesystem.  This method should be called before any other filesystem methods.
 * @param filesystem_path_prefix prefix to be prepended to all calls to filesystem_open.  This path should not end with `/`.
 */
void filesystem_init(const char* filesystem_path_prefix);
int filesystem_open(const char* path);
void filesystem_close(uint32_t handle);
int filesystem_read(void* buf, int size, int count, int handle);

#endif
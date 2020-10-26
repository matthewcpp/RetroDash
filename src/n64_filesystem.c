#include "filesystem.h"

#include <libdragon.h>

void filesystem_init(const char* filesystem_path_prefix) {
    dfs_init( DFS_DEFAULT_LOCATION );
    (void)filesystem_path_prefix;
}

int filesystem_open(const char* path) {
    return dfs_open(path);
}

void filesystem_close(uint32_t handle) {
    dfs_close(handle);
}

int filesystem_read(void* buf, int size, int count, int handle) {
    return dfs_read(buf, size, count, handle);
}

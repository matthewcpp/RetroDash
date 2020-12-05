#include "../filesystem.h"

#include <libdragon.h>

#include <stdlib.h>
#include <string.h>

void filesystem_init(const char* filesystem_path_prefix) {
    dfs_init( DFS_DEFAULT_LOCATION );
    (void)filesystem_path_prefix;
}

int filesystem_open(const char* path) {
    char* full_path = malloc(strlen(path) + 1);
    strcpy(full_path, "/");
    strcat(full_path,path);

    int result = dfs_open(path);
    free(full_path);

    return result;
}

void filesystem_close(uint32_t handle) {
    dfs_close(handle);
}

int filesystem_read(void* buf, int size, int count, int handle) {
    return dfs_read(buf, size, count, handle);
}

void filesystem_seek(int handle, int offset, int origin) {
    dfs_seek(handle, offset, origin);
}

void filesystem_uninit() {
    // nothing to do here.
}

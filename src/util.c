#include "util.h"

#ifdef PLATFORM_XBOX
void massage_path(char* path, size_t path_len) {
    for (size_t i = 0; i < path_len; i++) {
        if (path[i] == '/')
            path[i] = '\\';
    }
}
#else
void massage_path(char* path, size_t path_len) {
    (void)path;
    (void)path_len;
}
#endif

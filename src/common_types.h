#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

typedef struct {
    char* name;
    mode_t perms;
    size_t size;
} dir_entry_t;

#endif

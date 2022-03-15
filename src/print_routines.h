#ifndef PRINT_ROUTINES_H
#define PRINT_ROUTINES_H

#include <dirent.h>
#include <stdbool.h>
#include <sys/types.h>

#include "common_types.h"

void set_entry_color(mode_t perms);
void set_default_color();
void print_folder_contents(dir_entry_t* entries, size_t n);
void outRWX(int perms, char* cR, char* cW, char* cX, char* c, char id);
void outPerms(mode_t perms);
void outFloatAs3Chars(float input);
void outHumanReadableSize(
    __off_t size, char* color,
    bool floatingNum); /* TODO: type to actual size type */
void print_folder_contents_detailed(dir_entry_t* entries, size_t n);

#endif

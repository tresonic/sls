#include <assert.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "common_types.h"
#include "print_routines.h"

int dir_alphasort(const struct dirent** a, const struct dirent** b)
{
    if ((*a)->d_type == (*b)->d_type)
        return strcoll((*a)->d_name, (*b)->d_name);
    else if ((*a)->d_type == DT_DIR)
        return -1;
    else if ((*b)->d_type == DT_DIR)
        return 1;
    return 0;
}

int filter_discard_dirs(const struct dirent* dirent)
{
    if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)
        return 0;
    return 1;
}

size_t read_folder(char* dir, dir_entry_t** entries, int include_hidden)
{
    struct dirent** namelist;
    int n;

    n = scandir(dir, &namelist, filter_discard_dirs, dir_alphasort);
    dir_entry_t* ents = calloc(n, sizeof(dir_entry_t));
    /*printf("reading folder - %i entries found\n", n);*/
    /*printf("addr: %p\n", namelist[0]->d_name);*/
    if (n < 0)
        perror("scandir");
    else {
        for (int i = 0; i < n; i++) {
            struct stat st;
            char path[260];
            path[0] = 0;
            if (strlen(dir) > 1) {
                strcpy(path, dir);
                strcat(path, "/");
            }
            strcat(path, namelist[i]->d_name);
            stat(path, &st);

            char* name = malloc(strlen(namelist[i]->d_name) + 1);
            strcpy(name, namelist[i]->d_name);
            ents[i].name = name;

            ents[i].perms = st.st_mode;
            ents[i].size = st.st_size;
            /*printf("size: %lu; %lu\n", st.st_size, ents[i].size);*/
            /*printf("addr match: %p %p\n", namelist[i]->d_name, ents[i].name);*/

            /*printf("name %i: %lu\n", i, ents[0].size);*/
            free(namelist[i]);
        }
        free(namelist);
    }
    *entries = ents;
    /*puts("reading folder finished");*/
    return n;
}

int main(int argc, char* argv[])
{
    dir_entry_t* entries;
    size_t n = 0;
    /*n = read_folder("/tmp", &entries, 0);*/
    if (argc == 2)
        n = read_folder(argv[1], &entries, 0);
    else if (argc == 1)
        n = read_folder(".", &entries, 0);
    return 0;
}

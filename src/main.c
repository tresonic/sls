#include <assert.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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

size_t read_folder(char* dir, dir_entry_t** entries, int exclude_hidden)
{
    struct dirent** namelist;
    int n;

    n = scandir(dir, &namelist, filter_discard_dirs, dir_alphasort);
    if (n < 0) {
        perror("scandir");
        exit(1);
    }

    int num_hidden = 0;
    for (int i = 0; i < n; i++) {
        if (namelist[i]->d_name[0] == '.' && exclude_hidden) {
            num_hidden++;
        }
    }

    dir_entry_t* ents = calloc(n - num_hidden, sizeof(dir_entry_t));
    int entry_idx = 0;

    for (int i = 0; i < n; i++) {
        if (namelist[i]->d_name[0] == '.' && exclude_hidden) {
            free(namelist[i]);
            continue;
        }

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
        ents[entry_idx].name = name;

        ents[entry_idx].perms = st.st_mode;
        ents[entry_idx].size = st.st_size;
        entry_idx++;

        free(namelist[i]);
    }
    free(namelist);

    *entries = ents;
    return n - num_hidden;
}

int main(int argc, char* argv[])
{
    dir_entry_t* entries;
    size_t n = 0;
    int opt = 0;

    int exclude_hidden_files = 0;
    int list_view = 0;

    while ((opt = getopt(argc, argv, "le")) != -1) {
        switch (opt) {
        case 'l':
            list_view = 1;
            break;
        case 'e':
            exclude_hidden_files = 1;
            break;
        }
    }

    if (optind == argc) {
        n = read_folder(".", &entries, exclude_hidden_files);
    } else {
        puts("reading specified folder");
        n = read_folder(argv[argc - 1], &entries, 0);
    }

    if (list_view)
        print_folder_contents_detailed(entries, n);
    else
        print_folder_contents(entries, n);

    return 0;
}

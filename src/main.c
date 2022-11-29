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

void helpOutput()
{
    printf("Simple (or Small) LiSt tool that has less features then common list tools but has colors\n");
    printf("\nKnown Options\n");
    printf("h | get a short help message\n");
    printf("l | output detailed information in a list\n");
    printf("e | exclude hidden files\n");
    printf("D | exclude all files\n");
}

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

size_t read_folder(char* dir, dir_entry_t** entries, int exclude_files)
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
        if (exclude_files == 3 && (namelist[i]->d_type != DT_DIR || namelist[i]->d_name[0] == '.')) {
            num_hidden++;
        } else if (exclude_files == 2 && namelist[i]->d_type != DT_DIR) {
            num_hidden++;
        } else if (exclude_files == 1 && namelist[i]->d_name[0] == '.') {
            num_hidden++;
        }
    }

    dir_entry_t* ents = calloc(n - num_hidden, sizeof(dir_entry_t));
    int entry_idx = 0;

    for (int i = 0; i < n; i++) {
        if (exclude_files == 3 && (namelist[i]->d_type != DT_DIR || namelist[i]->d_name[0] == '.')) {
            free(namelist[i]);
            continue;
        } else if (exclude_files == 2 && namelist[i]->d_type != DT_DIR) {
            free(namelist[i]);
            continue;
        } else if (exclude_files == 1 && namelist[i]->d_name[0] == '.') {
            free(namelist[i]);
            continue;
        }

        struct stat st;
        char path[260];
        path[0] = 0;
        if (strlen(dir) > 0) {
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

    int exclude_files = 0;
    int list_view = 0;

    while ((opt = getopt(argc, argv, "hleD")) != -1) {
        switch (opt) {
        case 'h':
            helpOutput();
            return 0;
            break;
        case 'l':
            list_view = 1;
            break;
        case 'e':
            exclude_files += 1;
            break;
        case 'D':
            exclude_files += 2;
            break;
        default:
            printf("use -h to list known options\n");
            return 0;
            break;
        }
    }

    if (optind == argc) {
        n = read_folder(".", &entries, exclude_files);
    } else {
        n = read_folder(argv[argc - 1], &entries, exclude_files);
    }

    if (list_view)
        print_folder_contents_detailed(entries, n);
    else
        print_folder_contents(entries, n);

    return 0;
}

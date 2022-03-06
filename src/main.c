#include <assert.h>
#include <dirent.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "colors.h"

typedef struct {
    char* name;
    mode_t perms;
    size_t size;
} dir_entry_t;

int filter_discard_dirs(const struct dirent* dirent)
{
    if (strcmp(dirent->d_name, ".") == 0
        || strcmp(dirent->d_name, "..") == 0)
        return 0;
    return 1;
}

size_t read_folder(char* dir, dir_entry_t** entries, int include_hidden)
{
    struct dirent** namelist;
    int n;

    n = scandir(dir, &namelist, filter_discard_dirs, alphasort);
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

void set_entry_color(mode_t perms)
{
    switch (perms & S_IFMT) {
    case S_IFREG: /* file */
        if (perms & S_IEXEC)
            printf("%s", LIGHT_RED);
        else
            printf("%s", LIGHT_GRAY);

        break;
    case S_IFBLK: /* block special file */
        printf("%s", WHITE);
        break;
    case S_IFCHR: /* character special file */
        printf("%s", WHITE);
        break;
    case S_IFDIR: /* directory */
        printf("%s", LIGHT_GREEN);
        break;
    case S_IFLNK: /* symlink */
        printf("%s", CYAN);
        break;
    case S_IFIFO: /* FIFO/pipe */
        printf("%s", WHITE);
        break;
    case S_IFSOCK: /* socket */
        printf("%s", WHITE);
        break;
    default: /* should be network? */
        printf("%s", WHITE);
        break;
    }
}

void set_default_color()
{
    printf("%s", DEFAULT_COLOR);
}

void print_folder_contents(dir_entry_t* entries, size_t n)
{
    for (int i = 0; i < n; i++) {
        set_entry_color(entries[i].perms);
        printf("%s    ", entries[i].name);
    }
    set_default_color();
}

void outRWX(int perms, char* cR, char* cW, char* cX, char* c, char id)
{
    if (perms > 3) {
        perms -= 4;
        printf("%sr", cR);
    } else {
        printf("%s-", c);
    }

    if (perms > 1) {
        perms -= 2;
        printf("%sw", cW);
    } else {
        printf("%s-", c);
    }

    if (perms) {
        printf("%s%c", cX, id);
    } else {
        printf("%s-", c);
    }
}

void outPerms(mode_t perms)
{
    switch (perms & S_IFMT) {
    case S_IFREG: /* file */
        printf("%s.", WHITE);
        break;
    case S_IFBLK: /* block special file */
        printf("%sb", WHITE);
        break;
    case S_IFCHR: /* character special file */
        printf("%sc", WHITE);
        break;
    case S_IFDIR: /* directory */
        printf("%sd", BLUE);
        break;
    case S_IFLNK: /* symlink */
        printf("%sl", CYAN);
        break;
    case S_IFIFO: /* FIFO/pipe */
        printf("%sp", WHITE);
        break;
    case S_IFSOCK: /* socket */
        printf("%ss", WHITE);
        break;
    default: /* should be network? */
        printf("%sn", WHITE);
        break;
    }

    size_t setId = (((perms & 7000) - 344) / 512); /* I'm not sure if this is right but I don't know how to test it (set UID bit | set-group-ID bit | sticky bit)*/
    /*printf("perms: %i\n", perms & 7000);*/
    /*if (setId > 3) {*/
    /*setId -= 4;*/
    /*outRWX((perms & S_IRWXU) / 64, YELLOW, LIGHT_RED, LIGHT_RED, LIGHT_GRAY, 's');*/
    /*} else {*/
    outRWX((perms & S_IRWXU) / 64, YELLOW, LIGHT_RED, LIGHT_GREEN, LIGHT_GRAY, 'x');
    /*}*/

    /*if (setId > 1) {*/
    /*setId -= 2;*/
    /*outRWX((perms & S_IRWXG) / 8, LIGHT_GRAY, RED, LIGHT_RED, LIGHT_GRAY, 's');*/
    /*} else {*/
    outRWX((perms & S_IRWXG) / 8, LIGHT_GRAY, RED, GREEN, LIGHT_GRAY, 'x');
    /*}*/

    /*if (setId) {*/
    /*outRWX(perms & S_IRWXO, LIGHT_GRAY, RED, LIGHT_PURPLE, LIGHT_GRAY, 't');*/
    /*} else {*/
    outRWX(perms & S_IRWXO, LIGHT_GRAY, RED, GREEN, LIGHT_GRAY, 'x');
    /*}*/
}

void outFloatAs3Chars(float input)
{
    bool isFloating = !((int)input == input);
    if ((int)input / 100)
        printf("%d", (int)input);
    else if ((int)input / 10)
        printf(" %d", (int)round(input));
    else if (isFloating)
        printf("%.1f", input);
    else
        printf("  %d", (int)input);
}

void outHumanReadableSize(__off_t size, char* color, bool floatingNum) /* TODO: type to actual size type */
{
    printf("%s", color);
    if ((int)(size / pow(10, 24))) {
        if (floatingNum) {
            outFloatAs3Chars(size / pow(10, 24));
            printf("%sY", GREEN);
        } else {
            outFloatAs3Chars((int)(size / pow(10, 24)));
            printf("%sY", GREEN);
        }
    } else if ((int)(size / pow(10, 21))) {
        if (floatingNum) {
            outFloatAs3Chars(size / pow(10, 21));
            printf("%sZ", GREEN);
        } else {
            outFloatAs3Chars((int)(size / pow(10, 21)));
            printf("%sZ", GREEN);
        }
    } else if ((int)(size / pow(10, 18))) {
        if (floatingNum) {
            outFloatAs3Chars(size / pow(10, 18));
            printf("%sE", GREEN);
        } else {
            outFloatAs3Chars((int)(size / pow(10, 18)));
            printf("%sE", GREEN);
        }
    } else if ((int)(size / pow(10, 15))) {
        if (floatingNum) {
            outFloatAs3Chars(size / pow(10, 15));
            printf("%sP", GREEN);
        } else {
            outFloatAs3Chars((int)(size / pow(10, 15)));
            printf("%sP", GREEN);
        }
    } else if ((int)(size / pow(10, 12))) {
        if (floatingNum) {
            outFloatAs3Chars(size / pow(10, 12));
            printf("%sT", GREEN);
        } else {
            outFloatAs3Chars((int)(size / pow(10, 12)));
            printf("%sT", GREEN);
        }
    } else if ((int)(size / pow(10, 9))) {
        if (floatingNum) {
            outFloatAs3Chars(size / pow(10, 9));
            printf("%sG", GREEN);
        } else {
            outFloatAs3Chars((int)(size / pow(10, 9)));
            printf("%sG", GREEN);
        }
    } else if ((int)(size / pow(10, 6))) {
        if (floatingNum) {
            outFloatAs3Chars(size / pow(10, 6));
            printf("%sM", GREEN);
        } else {
            outFloatAs3Chars((int)(size / pow(10, 6)));
            printf("%sM", GREEN);
        }
    } else if ((int)(size / pow(10, 3))) {
        if (floatingNum) {
            outFloatAs3Chars(size / pow(10, 3));
            printf("%sK", GREEN);
        } else {
            outFloatAs3Chars((int)(size / pow(10, 3)));
            printf("%sK", GREEN);
        }
    } else {
        outFloatAs3Chars(size);
        printf(" ");
    }
}

void print_folder_contents_detailed(dir_entry_t* entries, size_t n)
{
    for (int i = 0; i < n; i++) {
        outPerms(entries[i].perms);
        printf(" ");
        outHumanReadableSize(entries[i].size, LIGHT_GREEN, true);
        set_default_color();
        set_entry_color(entries[i].perms);
        printf(" %s\n", entries[i].name);
        set_default_color();
    }
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
    print_folder_contents_detailed(entries, n);
    return 0;
}

#include "print_routines.h"

#include <math.h>
#include <stdio.h>
#include <sys/stat.h>

#include "colors.h"

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

void set_default_color() { printf("%s", DEFAULT_COLOR); }

void print_folder_contents(dir_entry_t* entries, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        set_entry_color(entries[i].perms);
        printf("%s    ", entries[i].name);
    }
    set_default_color();
    putchar('\n');
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

    size_t setId = (((perms & 7000) - 344) / 512); /* I'm not sure if this is right but I
   don't know how to test it (set UID bit | set-group-ID bit | sticky bit)*/
    /*printf("perms: %i\n", perms & 7000);*/
    /*if (setId > 3) {*/
    /*setId -= 4;*/
    /*outRWX((perms & S_IRWXU) / 64, YELLOW, LIGHT_RED, LIGHT_RED, LIGHT_GRAY,
   * 's');*/
    /*} else {*/
    outRWX((perms & S_IRWXU) / 64, YELLOW, LIGHT_RED, LIGHT_GREEN, LIGHT_GRAY,
        'x');
    /*}*/

    /*if (setId > 1) {*/
    /*setId -= 2;*/
    /*outRWX((perms & S_IRWXG) / 8, LIGHT_GRAY, RED, LIGHT_RED, LIGHT_GRAY,
   * 's');*/
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

void outHumanReadableSize(__off_t size, char* color,
    bool floatingNum) /* TODO: type to actual size type */
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
    for (size_t i = 0; i < n; i++) {
        outPerms(entries[i].perms);
        printf(" ");
        if ((entries[i].perms & S_IFMT) != S_IFDIR)
            outHumanReadableSize(entries[i].size, LIGHT_GREEN, true);
        else
            printf("%sDIR %s", LIGHT_GREEN, DEFAULT_COLOR);
        set_default_color();
        set_entry_color(entries[i].perms);
        printf(" %s\n", entries[i].name);
        set_default_color();
    }
}

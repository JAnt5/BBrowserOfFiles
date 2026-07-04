// Source - https://stackoverflow.com/a/29486795
// Posted by David C. Rankin
// Retrieved 2026-07-04, License - CC BY-SA 3.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  /* opendir  */
#include <dirent.h>     /* opendir, readdir, scandir  */
#include <errno.h>

#ifndef _BSD_SOURCE     /* for scandir sort routines  */
#define _BSD_SOURCE
#endif /* _BSD_SOURCE */

int sdfilt (const struct dirent *de);

int main (int argc, char **argv) {

    if (argc < 2 )
        fprintf (stderr, "warning: usage: %s [dirname (default '.')][mode (default 14)\n", argv[0]);


    char *dname = (argc > 1) ? argv[1] : ".";   /* directory name to get listing of */
    struct dirent **namelist = NULL;            /* dirent structure to hold listing */
    int ndir = 0;                               /* num dirs scandir returns, -1 err */
    size_t it = 0;                              /* simple iterator for dir list     */

    /* call scandir to fill pointer to array of dirent entries  */
    if ((ndir = scandir (dname, &namelist, sdfilt, alphasort)) < 0)
    {
        perror("scandir");  /* throw error & return on failure  */
        return 1;
    }

    /* print each of the entries in alphasort order  */
    printf ("\nscandir example (alphasort):\n\n");
    for (it = 0; it < ndir; it++)
        printf("  nl[%2zu] %s\n", it, namelist[it]->d_name);

    free(namelist);

    printf ("\n");

    return 0;
}

/* simple scandir filter that omit strings and
dot files '.' and '..' from dirent entries */
int sdfilt (const struct dirent *de)
{
    if (strcmp (de->d_name, ".") == 0 || strcmp (de->d_name, "..") == 0)
        return 0;
    else
        return 1;
}

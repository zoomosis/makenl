/* $Id: oswatchd.c,v 1.1 2013/09/20 21:07:05 ajleary Exp $ */

/*
 *  os_chdir() - Watcom C
 *
 *  Workaround for systems where chdir() will not accept a trailing \ character unless
 *  it's the root of the drive.
 */

#include <ctype.h>

#if defined(__MSDOS__) || defined(__OS2__) || defined(WIN32)

static int validdriveletter(int drive)
{
    char driveletters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return strchr(driveletters, toupper(drive)) != NULL;
}

int os_chdir(char *path)
{
    char *newpath, *p;
    int rc;

    mklog(LOG_DEBUG, "os_chdir path='%s'", path);

    if (path == NULL || path[0] == '\0')
    {
        /* null pointer or empty string; do nothing */
        return 0;
    }

    if (path[0] == '\\' && path[1] == '\0')
    {
        /* lone '\' path is OK */
        return chdir(path);
    }

    if (validdriveletter(path[0]) && path[1] == ':' && path[2] == '\\' && path[3] == '\0')
    {
        /* "x:\" is OK too */

        return chdir(path);
    }

    /* copy path and modify it */

    newpath = malloc(strlen(path) + 1);

    if (newpath == NULL)
    {
        /* out of memory */
        return 1;
    }

    strcpy(newpath, path);

    p = newpath + strlen(newpath) - 1;

    if (*p == '\\')
    {
        /* strip trailing \ */

        *p = '\0';
    }

    mklog(LOG_DEBUG, "os_chdir newpath='%s'", newpath);
    rc = chdir(newpath);
    free(newpath);
    return rc;
}

#else

int os_chdir(char *path)
{
    return chdir(path);
}

#endif

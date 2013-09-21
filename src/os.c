/* os.c -- Operating system dependant functions for makenl */

/* $Id: os.c,v 1.5 2013/09/21 08:58:55 ozzmosis Exp $ */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "fileutil.h"
#include "makenl.h"
#include "os.h"
#include "mklog.h"

/* Definitions for:                                               */
/* __GNUC__     GNU C Compiler under DOS and OS/2 and linux       */
/* __unix__     GNU C Compiler under Unix (tested with linux)     */
/* __EMX__      emx+gcc for DOS/OS2                               */
/* __WATCOMC__  Watcom C Compiler 10.0b under DOS, OS/2 and Win32 */
/* __TURBOC__   Turbo C 2.00 and above, Borland C, under DOS      */

#ifndef OSFUL
#error "No os_fullpath for this compiler/OS, giving up!"
#endif
#include OSFUL

#ifndef OSFND
#error "No os_findfirst/os_findnext/os_findclose for this compiler/OS, giving up!"
#endif
#include OSFND

/* os_deslashify */
#include OSDSL
/* os_fulldir */
#include OSFLD
/* os_findfile */
#include OSFF
/* os_append_slash */
#include OSAPS
/* os_remove_slash */
#include OSRMS

/* os_spawn */
#include OSEXC

#ifndef HAVE_STRUPR
#include "osgenupr.c"
#endif

#ifdef USE_OWN_FGETS
#include USE_OWN_FGETS
#endif

char *os_file_getname(const char *path)
{
    const char *p;

    p = path;
    while (*path != 0)
        switch (*path++)
        {
        case ':':
        case '/':
        case '\\':
            p = path;           /* Note that PATH has been incremented */
            break;
        }
    return (char *)p;
}

/*
 *  os_chdir()
 *
 *  Workaround for systems where chdir() will not accept a trailing \ character unless
 *  it's the root of the drive.
 */

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

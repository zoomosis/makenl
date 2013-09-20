/* os.c -- Operating system dependant functions for makenl */

/* $Id: os.c,v 1.4 2013/09/20 21:07:04 ajleary Exp $ */

#include <stdio.h>
#include <string.h>
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

/* os_chdir */
#include OSCHD
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

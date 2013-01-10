/* $Id: osdosfnd.c,v 1.1 2009/01/08 20:07:47 mbroek Exp $ */

#include <dos.h>
#include "unused.h"

char *os_findfirst(struct _filefind *pff, const char *path,
                   const char *mask)
{
    int rc;
    char tmp[MAXPATH];

    strcpy(tmp, path);
    os_append_slash(tmp);
    strcat(tmp, mask);
    rc = findfirst(tmp, &pff->fileinfo,
                   FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH);
    if (rc == 0)
        return pff->fileinfo.ff_name;

    return NULL;
}


char *os_findnext(struct _filefind *pff)
{
    if (findnext(&pff->fileinfo) == 0)
        return pff->fileinfo.ff_name;

    return NULL;
}


void os_findclose(struct _filefind *pff)
{
    unused(pff);
}
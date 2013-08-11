/* $Id: oswatfnd.c,v 1.2 2013/08/11 23:05:43 ozzmosis Exp $ */

#define HAVE_OS_FIND
#include <string.h>
#include "unused.h"

char *os_findfirst(struct _filefind *pff, const char *path,
                   const char *mask)
{
    unsigned rc;
    char tmp[MYMAXPATH];

    strcpy(tmp, path);
    os_append_slash(tmp);
    strcat(tmp, mask);
    rc = _dos_findfirst(tmp,
                        _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM |
                        _A_ARCH, &pff->fileinfo);
    if (rc == 0)
        return pff->fileinfo.name;

    return NULL;
}


char *os_findnext(struct _filefind *pff)
{
    if (_dos_findnext(&pff->fileinfo) == 0)
        return pff->fileinfo.name;

    return NULL;
}


void os_findclose(struct _filefind *pff)
{
#if defined(__TURBOC__) && defined(__OS2__)
    unused(pff);
#else
    _dos_findclose(&pff->fileinfo);
#endif
}

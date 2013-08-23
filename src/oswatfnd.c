/* $Id: oswatfnd.c,v 1.3 2013/08/23 14:55:00 ozzmosis Exp $ */

#define HAVE_OS_FIND

#include <direct.h>
#include "patmat.h"

char *os_findfirst(struct _filefind *pff, const char *path,
                   const char *mask)
{
    strcpy(pff->path, path);
    os_remove_slash(pff->path);
    strcpy(pff->mask, mask);

    if ((pff->dirp = opendir(pff->path)) != NULL)
    {
        char *p;

        if ((p = os_findnext(pff)) != NULL)
            return p;
    }

    closedir(pff->dirp);
    pff->dirp = NULL;
    return NULL;
}

char *os_findnext(struct _filefind *pff)
{
    int matchresult;

    for (;;)
    {
        if ((pff->pentry = readdir(pff->dirp)) == NULL)
            return NULL;

        matchresult = patmat(pff->pentry->d_name, pff->mask);
        if (matchresult == 0)
            return pff->pentry->d_name;
    }
}

void os_findclose(struct _filefind *pff)
{
    if (pff->dirp)
        closedir(pff->dirp);
    pff->dirp = NULL;
}

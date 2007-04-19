/* $Id: osgnufnd.c,v 1.2 2004-07-11 09:29:14 ozzmosis Exp $ */

#define _GNU_SOURCE
#include <dirent.h>
#include <stdlib.h>
#include <fnmatch.h>
char *os_findfirst(struct _filefind *pff, const char *path,
                   const char *mask)
{
    strcpy(pff->path, path);
    os_remove_slash(pff->path);
    strcpy(pff->mask, mask);
    pff->flags = FNM_NOESCAPE;

#ifdef __EMX__
    pff->flags |= (_osmode == OS2_MODE) ? _FNM_OS2 : _FNM_DOS;
    pff->flags |= _FNM_IGNORECASE;
#endif

#if defined(__unix__) || defined(__MSDOS__)
    pff->flags |= FNM_CASEFOLD;
#endif

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

        matchresult = fnmatch(pff->mask, pff->pentry->d_name, pff->flags);
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

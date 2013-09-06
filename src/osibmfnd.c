/* $Id: osibmfnd.c,v 1.1 2013/08/23 14:50:01 ozzmosis Exp $ */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define INCL_DOS
#include <os2.h>

#include "patmat.h"

#define SEARCH_ATTR (FILE_HIDDEN|FILE_SYSTEM|FILE_DIRECTORY)

static DIR *opendir(const char *dirname)
{
    char *name;
    int len;
    DIR *dir;
    ULONG nfiles;
    APIRET apiret;

    len = strlen(dirname);
    if ((name = malloc(len + 5)) == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    strcpy(name, dirname);
    if (len-- && name[len] != ':' && name[len] != '\\' && name[len] != '/')
        strcat(name, "\\*.*");
    else
        strcat(name, "*.*");

    if ((dir = malloc(sizeof(DIR))) == NULL)
    {
        errno = ENOMEM;
        free(name);
        return NULL;
    }

    dir->hdir = HDIR_CREATE;
    nfiles = 256;
    if ((apiret = DosFindFirst((PSZ) name, &dir->hdir, SEARCH_ATTR,
                               (PVOID) & dir->buf[0], sizeof(dir->buf),
                               &nfiles, FIL_STANDARD)) != 0)
    {
        free(name);
        free(dir);
        return NULL;
    }

    dir->dirname = name;
    dir->nfiles = (unsigned)nfiles;
    dir->bufp = &dir->buf[0];
    return dir;
}

static struct dirent *readdir(DIR * dir)
{
    ULONG nfiles;
    FILEFINDBUF3 *ff;

    if (dir->nfiles == 0)
    {
        nfiles = 256;
        if (DosFindNext(dir->hdir, (PFILEFINDBUF) & dir->buf[0],
                        sizeof(dir->buf), &nfiles) != 0)
            return NULL;
        dir->nfiles = (unsigned)nfiles;
        dir->bufp = &dir->buf[0];
    }

    ff = (FILEFINDBUF3 *) (dir->bufp);
    dir->bufp += (int)ff->oNextEntryOffset;
    dir->nfiles--;

    return ((struct dirent *)&ff->achName[0]);
}

static int closedir(DIR * dir)
{
    if (dir == NULL)
    {
        errno = EBADF;
        return -1;
    }

    DosFindClose(dir->hdir);
    free(dir->dirname);
    free(dir);
    return 0;
}

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

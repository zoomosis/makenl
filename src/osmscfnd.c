/* $Id: osmscfnd.c,v 1.1 2004/07/18 10:51:37 ozzmosis Exp $ */

#include <io.h>

char *os_findfirst(struct _filefind *pff, const char *path,
                   const char *mask)
{
    char tmp[FILENAME_MAX];

    strcpy(tmp, path);
    os_append_slash(tmp);
    strcat(tmp, mask);
    pff->handle = _findfirst(tmp, &pff->fileinfo);

    if (pff->handle == 0)
    {
        return NULL;
    }
    
    return pff->fileinfo.name;
}


char *os_findnext(struct _filefind *pff)
{
    if (_findnext(pff->handle, &pff->fileinfo) == 0)
        return pff->fileinfo.name;

    return NULL;
}


void os_findclose(struct _filefind *pff)
{
    _findclose(pff->handle);
}

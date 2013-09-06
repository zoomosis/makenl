/* $Id: osgenff.c,v 1.2 2013/09/04 00:58:31 ozzmosis Exp $ */

char *os_findfile(struct _filefind *pff, const char *path,
                  const char *mask)
{
    char *p;

    p = os_findfirst(pff, path, mask);

    if (p)
    {
        strcpy(pff->path, p);
        os_findclose(pff);
        return pff->path;
    }

    return NULL;
}

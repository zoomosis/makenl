/* $Id: osgenff.c,v 1.1.1.1 2009/01/08 20:07:47 mbroek Exp $ */

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

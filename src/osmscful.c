/* $Id: osmscful.c,v 1.2 2013/09/05 14:00:13 ozzmosis Exp $ */

#define HAVE_OS_FULLPATH

/* make an absolute path from given relative path */
int os_fullpath(char *dst, const char *src, size_t bufsiz)
{
    if (!_fullpath(dst, src, bufsiz))
    {
        return -1;
    }
    os_deslashify(dst);
    return 0;
}

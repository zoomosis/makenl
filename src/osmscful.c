/* $Id: osmscful.c,v 1.1 2004/07/18 10:51:37 ozzmosis Exp $ */

#define HAVE_OS_FULLPATH

/* make an absolute path from given relative path */
int os_fullpath(char *dst, const char *src, size_t bufsiz)
{
    if (!_fullpath(dst, src, bufsiz))
    {
        return -1;
    }
    os_filecanonify(dst);
    return 0;
}

/* $Id: osmscful.c,v 1.1.1.1 2009/01/08 20:07:47 mbroek Exp $ */

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

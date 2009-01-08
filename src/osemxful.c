/* $Id: osemxful.c,v 1.1.1.1 2009/01/08 20:07:47 mbroek Exp $ */

#define HAVE_OS_FULLPATH

/* make an absolute path from given relative path */
int os_fullpath(char *dst, const char *src, size_t bufsiz)
{
    int result;

    result = _fullpath(dst, src, bufsiz);
    os_filecanonify(dst);
    return result;
}

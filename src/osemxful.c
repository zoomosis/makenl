/* $Id: osemxful.c,v 1.2 2004/07/11 09:29:14 ozzmosis Exp $ */

#define HAVE_OS_FULLPATH

/* make an absolute path from given relative path */
int os_fullpath(char *dst, const char *src, size_t bufsiz)
{
    int result;

    result = _fullpath(dst, src, bufsiz);
    os_filecanonify(dst);
    return result;
}

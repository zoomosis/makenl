/* $Id: oswatful.c,v 1.2 2004/07/11 09:29:15 ozzmosis Exp $ */

/* make an absolute path from given relative path */
int os_fullpath(char *dst, const char *src, size_t bufsiz)
{
    if (!_fullpath(dst, src, bufsiz))
        return -1;
    os_filecanonify(dst);
    return 0;
}

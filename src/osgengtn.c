/* $Id: osgengtn.c,v 1.2 2004/07/11 09:29:14 ozzmosis Exp $ */

char *os_file_getname(const char *path)
{
    const char *p;

    p = path;
    while (*path != 0)
        switch (*path++)
        {
        case ':':
        case '/':
        case '\\':
            p = path;           /* Note that PATH has been incremented */
            break;
        }
    return (char *)p;
}

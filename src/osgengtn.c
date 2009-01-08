/* $Id: osgengtn.c,v 1.1.1.1 2009/01/08 20:07:47 mbroek Exp $ */

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

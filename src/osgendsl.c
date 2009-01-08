/* $Id: osgendsl.c,v 1.1 2009/01/08 20:07:47 mbroek Exp $ */

/* converts slashes to backslashes for pathnames under DOS or EMX */
char *os_deslashify(char *name)
{
    char *p;

    p = name;
    while (*p != 0)
        if (*p == '/')
            *p++ = '\\';
        else
            ++p;

    return name;
}

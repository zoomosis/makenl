/* $Id: osgendsl.c,v 1.2 2004/07/11 09:29:14 ozzmosis Exp $ */

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

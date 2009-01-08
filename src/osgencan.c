/* $Id: osgencan.c,v 1.1.1.1 2009/01/08 20:07:47 mbroek Exp $ */

char *os_filecanonify(char *s)
{
    os_deslashify(s);
    return s;
}

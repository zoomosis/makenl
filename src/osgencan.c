/* $Id: osgencan.c,v 1.3 2004/07/13 08:33:19 ozzmosis Exp $ */

char *os_filecanonify(char *s)
{
    os_deslashify(s);
    return s;
}

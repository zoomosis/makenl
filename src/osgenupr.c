/* $Id: osgenupr.c,v 1.2 2004/07/11 09:29:14 ozzmosis Exp $ */

#include <ctype.h>

char *strupr(char *string)
{
    char *mystr = string;

    while (*string)
    {
        *string = toupper((unsigned char)*string);
        string++;
    }
    return mystr;
}

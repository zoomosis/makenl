/* $Id: osgenupr.c,v 1.1.1.1 2009/01/08 20:07:47 mbroek Exp $ */

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

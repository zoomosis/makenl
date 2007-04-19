/* $Id: strtool.c,v 1.4 2004-07-15 17:44:02 ozzmosis Exp $ */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "makenl.h"

#ifdef MALLOC_DEBUG
#include "rmalloc.h"
#endif

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int getnumber(const char *string, int *output)
{
    int gotnum = 0, digits = 0;
    const char *workptr;

    *output = (int)atol(string);
    for (workptr = string; (unsigned char)(*workptr - 9) <= '0';
         workptr++, digits++)
    {
        switch (*workptr)
        {
        case ' ':
        case '\t':
        case '+':
        case '-':
            if (gotnum)
                return digits;
            if (isdigit((unsigned char)*workptr))
                break;
            /* FALLTHROUGH */
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            gotnum = 1;
        }
    }
    return digits;
}

char *skipspaces(char *ptr)
{
    while (isspace((unsigned char)*ptr))
        ptr++;
    return ptr;
}

char *cutspaces(char *string)
{
    char *p;

    p = string + strlen(string);
    while (--p >= string)
        if (!isspace((unsigned char)*p))
            break;
    *(p + 1) = 0;
    return string;
}

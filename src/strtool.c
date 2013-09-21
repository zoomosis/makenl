/* $Id: strtool.c,v 1.3 2013/09/21 09:50:50 ozzmosis Exp $ */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "strtool.h"

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

char *strupper(char *string)
{
    char *orig;

    if (string == NULL)
    {
        return NULL;
    }

    orig = string;

    while (*string != '\0')
    {
        *string = toupper((unsigned char) *string);
        string++;
    }

    return orig;
}

/* $Id: strtool.c,v 1.7 2013/09/25 19:39:07 ozzmosis Exp $ */

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
        *string = (char) toupper((int) *string);
        string++;
    }

    return orig;
}


/*
 *  strlcpy() and strlcat()
 *
 *  From http://www.di-mgt.com.au/cprog.html#strlcpy
 *
 *  It is not wise to use the ANSI standard strcpy and strcat functions
 *  because of the possibility of unchecked buffer overflows. The strncpy
 *  function has the problem that it may not properly terminate the string.
 *  The following strlcpy and strlcat functions are simple implementations
 *  that overcome the problems of their original ANSI ancestors
 *  (last updated 2011-12-11).
 *
 *  These functions are guaranteed not to cause a buffer overflow and the
 *  result is always null-terminated. If there is not enough room in the
 *  destination buffer, then it truncates the output. The return value is
 *  the number of bytes it should have copied, so you have a roundabout
 *  way of checking. The value of bufsize is the actual allocated size of
 *  the buffer, so you can use sizeof(buf) (providing buf is not a pointer).
 *
 *  For more on the background to these functions see strlcpy and strlcat -
 *  consistent, safe, string copy and concatenation by Todd C. Miller and
 *  Theo de Raadt.
 *
 *  The above solutions do have the disadvantage that they always do at
 *  least a double pass along the strings, and so are only half as
 *  efficient as the originals which don't. This is really only a problem
 *  for very long strings.
 */

#ifndef HAVE_STRLCPY

size_t strlcpy(char *d, const char *s, size_t bufsize)
{
    size_t len;
    size_t ret;

    if (!d || !s || (int)bufsize <= 0) return 0;
    len = strlen(s);
    ret = len;
    if (len >= bufsize) len = bufsize-1;
    memcpy(d, s, len);
    d[len] = 0;

    return ret;
}

#endif

#ifndef HAVE_STRLCAT

size_t strlcat(char *d, const char *s, size_t bufsize)
{
    size_t len1;
    size_t len2;
    size_t ret;

    if (!d || !s || (int)bufsize <= 0) return 0;
    len1 = strlen(d);
    len2 = strlen(s);
    ret = len1 + len2;
    if (len1+len2 >= bufsize) 
        len2 = bufsize - (len1 + 1);
    if (len2 > 0) 
    {
        memcpy(d+len1, s, len2);
        d[len1+len2] = 0;
    }
    return ret;
}

#endif

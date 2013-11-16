/* $Id: strtool.h,v 1.4 2013/09/25 19:46:00 ozzmosis Exp $ */

#ifndef __STRTOOL_H__
#define __STRTOOL_H__

int getnumber(const char *string, int *output);
char *skipspaces(char *ptr);
char *cutspaces(char *string);
char *strupper(char *string);

size_t strlcpy(char *d, const char *s, size_t bufsize);
size_t strlcat(char *d, const char *s, size_t bufsize);

/* necessary for some C implementations where printf("%s\n", NULL) would cause a segfault */

#define make_str_safe(x) (x)?(x):"(null)"

#endif

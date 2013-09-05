/* $Id */

#ifndef __STRTOOL_H__
#define __STRTOOL_H__

int getnumber(const char *string, int *output);
char *skipspaces(char *ptr);
char *cutspaces(char *string);

/* necessary for some C implementations where printf("%s\n", NULL) would cause a segfault */

#define make_str_safe(x) (x)?(x):"(null)"

#endif

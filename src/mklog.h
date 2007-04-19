/* $Id: mklog.h,v 1.3 2004-09-04 21:26:37 mbroek Exp $ */

#define MAKE_SS(x) (x)?(x):"(null)"

extern int loglevel;

void mklog(int, const char *, ...);


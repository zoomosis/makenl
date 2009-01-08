/* $Id: mklog.h,v 1.1 2009/01/08 20:07:47 mbroek Exp $ */

#define MAKE_SS(x) (x)?(x):"(null)"

extern int loglevel;

void mklog(int, const char *, ...);


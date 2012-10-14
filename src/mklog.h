/* $Id: mklog.h,v 1.4 2012/10/14 14:49:17 ozzmosis Exp $ */

#define LOG_ERROR 0  /* error messages; write to stderr */
#define LOG_INFO  1  /* informational messages; write to stdout */
#define LOG_DEBUG 2  /* debug messages; write to stderr if run with "makenl -d" */

extern int loglevel;

void mklog(int level, const char *format, ...);

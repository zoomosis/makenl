/* $Id: mklog.h,v 1.5 2012/10/14 15:24:21 ozzmosis Exp $ */

#define LOG_ERROR   0  /* error messages; write to stderr */
#define LOG_INFO    1  /* informational messages; write to stdout */
#define LOG_LOGONLY 2  /* informational messages; only write to log unless run with "makenl -d" */
#define LOG_DEBUG   3  /* debug messages; write to stderr if run with "makenl -d" */

extern int loglevel;

void mklog(int level, const char *format, ...);

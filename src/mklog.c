/* $Id: mklog.c,v 1.5 2012/10/12 23:01:30 ozzmosis Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

#ifdef __unix__
#include <unistd.h>
#endif

#include "makenl.h"
#include "mklog.h"
#include "fileutil.h"

#ifdef MALLOC_DEBUG
#include "rmalloc.h"
#endif

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int loglevel = 1;

static char *logmark = "?+-dD";

static char *date_str(void)
{
    time_t now;
    struct tm ptm;
    static char buf[128];

    now = time(NULL);
    ptm = *localtime(&now);
    strftime(buf, sizeof buf, "%d-%b-%Y %H:%M:%S", &ptm);
    return buf;
}

void mklog(int level, const char *format, ...)
{
    char outstr[4096];
    va_list va_ptr;
    FILE *fp;
    
    if (level > loglevel)
    {
	/* log level is higher than global log level */
	return;
    }
    
    if (*LogFile == '\0')
    {
	/* empty log filename */
	return;
    }
    
    if (format == NULL)
    {
	/* avoid segfault */
	return;
    }
    
    /* Open in textmode, gives the correct lineendings on all OSes. */

    fp = fopen(LogFile, "a");

    if (fp == NULL)
    {
	die(0xFF, 1, "Cannot open logfile \"%s\"", LogFile);
    }

    va_start(va_ptr, format);
    vsnprintf(outstr, sizeof outstr, format, va_ptr);
    va_end(va_ptr);

#if defined(__unix__)
    fprintf(fp, "%c %s makenl[%d] ", logmark[level], date_str(), getpid());
#else
    fprintf(fp, "%c %s makenl: ", logmark[level], date_str());
#endif
    fprintf(fp, "%s", *outstr == '$' ? outstr+1 : outstr);

    if (*outstr == '$')
    {
	fprintf(fp, ": %s\n", strerror(errno));
    }
    else
    {
	fputc('\n', fp);
    }
    
    fclose(fp);
}

/* $Id: mklog.c,v 1.10 2012/10/14 13:28:33 ozzmosis Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

#ifdef __unix__
# include <unistd.h>
#endif

#include "makenl.h"
#include "mklog.h"
#include "fileutil.h"

#ifdef MALLOC_DEBUG
# include "rmalloc.h"
#endif

#ifdef DMALLOC
# include "dmalloc.h"
#endif

static int logopened = 0;

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

    fp = NULL;
    
    if (format == NULL)
    {
	/* avoid segfault */
	return;
    }

    /* Open in textmode, gives the correct lineendings on all OSes. */

    if (*LogFile != '\0')
    {
	fp = fopen(LogFile, "a");

	if (fp == NULL)
	{
	    die(0xFF, "Cannot open logfile \"%s\"", LogFile);
	    return;
        }
    }

    va_start(va_ptr, format);
    vsnprintf(outstr, sizeof outstr, format, va_ptr);
    va_end(va_ptr);

    if (fp != NULL)
    {
        if (logopened == 0)
        {
            /* if this is the first log entry, start with an empty line before it */
            logopened = 1;
            fputc('\n', fp);
        }

#if defined(__unix__)
	fprintf(fp, "%c %s makenl[%d] ", logmark[level], date_str(), getpid());
#else
	fprintf(fp, "%c %s makenl: ", logmark[level], date_str());
#endif
        fprintf(fp, "%s", *outstr == '$' ? outstr + 1 : outstr);
    }

    if (level == 0 || MakenlDebug)
    {
	fprintf(stderr, "%s", *outstr == '$' ? outstr + 1 : outstr);
    }

    if (fp != NULL)
    {
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

    if (level == 0 || MakenlDebug)
    {
	if (*outstr == '$')
	{
	    fprintf(stderr, ": %s\n", strerror(errno));
        }
	else
	{
	    fputc('\n', stderr);
        }
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

#include "makenl.h"
#include "mklog.h"
#include "fileutil.h"
#include "os.h"
#include "snprintf.h"

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

static void logwrite(int level, char *outstr)
{
    int saved_errno = errno;
    static int logopened = 0;
    FILE *log_fp;
    FILE *std_fp;
    int need_strerror;

    std_fp = stderr;
    need_strerror = 0;

    /* if our error message begins with $, also log the error message from strerror() */

    if (*outstr == '$')
    {
        need_strerror = 1;
        outstr++;
    }

    /* write to stderr/stdout */

    if (level == LOG_INFO)
    {
        std_fp = stdout;
    }

    if (level < LOG_LOGONLY || debug_mode)
    {
        fprintf(std_fp, "%s", outstr);

        if (need_strerror)
        {
            fprintf(std_fp, ": %s\n", xstrerror(saved_errno));
        }
        else
        {
            fputc('\n', std_fp);
        }
    }

    if (!debug_mode && level == LOG_DEBUG && loglevel < 2)
    {
        /*
         *  if we're not running in debug mode (makenl -d), don't write
         *  debugging messages to the log file unless loglevel set to >1
         */
        return;
    }

    if (*LogFile == '\0')
    {
        /* log filename was not set */
        return;
    }

    /* write to logfile */

    log_fp = fopen(LogFile, "a");

    if (log_fp == NULL)
    {
        fprintf(stderr, "Cannot open logfile '%s'\n", LogFile);
        exit(255);
    }

    if (logopened == 0)
    {
        /* if this is the first log entry, write an empty line before it */
        logopened = 1;
        fputc('\n', log_fp);
    }

#ifdef HAVE_GETPID
    fprintf(log_fp, "%c %s makenl[%d] ", logmark[level], date_str(), (int) getpid());
#else
    fprintf(log_fp, "%c %s makenl: ", logmark[level], date_str());
#endif

    fprintf(log_fp, "%s", outstr);

    if (need_strerror)
    {
        fprintf(log_fp, ": %s\n", xstrerror(saved_errno));
    }
    else
    {
        fputc('\n', log_fp);
    }

    fclose(log_fp);
}

void mklog(int level, const char *format, ...)
{
    char outstr[4096];
    va_list va_ptr;

    if (format == NULL)
    {
        fprintf(stderr, "NULL format string for va_start()\n");
        exit(255);
    }

    va_start(va_ptr, format);
    vsnprintf(outstr, sizeof outstr, format, va_ptr);
    va_end(va_ptr);

    logwrite(level, outstr);
}

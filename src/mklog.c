/* $Id: mklog.c,v 1.2 2004/09/03 21:46:23 mbroek Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

#include "makenl.h"
#include "mklog.h"
#include "fileutil.h"

#ifdef MALLOC_DEBUG
#include "rmalloc.h"
#endif

#ifdef DMALLOC
#include "dmalloc.h"
#endif


int loglevel = 0;

static char *mon[] = {
	(char *)"Jan",(char *)"Feb",(char *)"Mar",
	(char *)"Apr",(char *)"May",(char *)"Jun",
	(char *)"Jul",(char *)"Aug",(char *)"Sep",
	(char *)"Oct",(char *)"Nov",(char *)"Dec"
};


char *date(void);
char *date(void)
{
    struct tm   ptm;
    time_t      now;
    static char buf[20];

    now = time(NULL);
    ptm = *localtime(&now);
    sprintf(buf,"%02d-%s-%04d %02d:%02d:%02d", ptm.tm_mday, mon[ptm.tm_mon], ptm.tm_year+1900,
				                            ptm.tm_hour, ptm.tm_min, ptm.tm_sec);
    return(buf);
}



void mklog(int level, const char *format, ...)
{
    char	*outstr;
    va_list     va_ptr;
    FILE	*fp;
    
    if ((level > loglevel) || (strlen(LogFile) == 0))
    {
	return;
    }

    if ((fp = fopen(LogFile, "a")) == NULL)
    {
	die(0xFF, 1, "Cannot open logfile \"%s\"", LogFile);
    }

    outstr = calloc(4096, sizeof(char));

    va_start(va_ptr, format);
    vsprintf(outstr, format, va_ptr);
    va_end(va_ptr);

    fprintf(fp, "%d %s makenl[%d] ", level, date(), getpid());
    fprintf(fp, *outstr == '$' ? outstr+1 : outstr);
    if (*outstr == '$')
	fprintf(fp, ": %s\n", strerror(errno));
    else
	fprintf(fp, "\n");
    fflush(fp);
    fclose(fp);

    free(outstr);
}


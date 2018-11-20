/* $Id: output.c,v 1.15 2016/12/02 22:20:06 ajleary Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "makenl.h"
#include "fts5.h"
#include "crc16.h"
#include "fileutil.h"
#include "mklog.h"
#include "strtool.h"
#include "snprintf.h"

char ErrorMessage[linelength];

int OutputErrorLine(FILE * file, const char *pre, const char *wrongy,
  const char *post, unsigned short *crc)
{
    char *linebuf;
    int bufsize;
    int fputs_result;

    if (FTS5Keyword)            /* Do we have an Nodelist line to be
                                   printed also? */
        if (OutputFTS5Line(file, pre, post, crc) == EOF)
            return EOF;

    bufsize = strlen(pre) + strlen(wrongy) + 4 + strlen(ErrorMessage) + strlen(post) + 1;
    linebuf = malloc(bufsize);
    if (!linebuf)
        die(253, "No memory left for error message buffer!\n");
    snprintf(linebuf, bufsize, "%s%s -- %s%s", pre, wrongy, ErrorMessage, post);
    if (crc)
        *crc = CRC16String(linebuf, *crc);
    fputs_result = fputs(linebuf, file);
    mklog(LOG_LOGONLY, "%s%s -- %s", pre, wrongy, ErrorMessage);
    free(linebuf);
    return fputs_result;
}

int
OutputFTS5Line(FILE * file, const char *prefix, const char *postfix,
               unsigned short *crc)
{
    char *linebuf;
    int bufsize;
    int fputs_result;

    bufsize = strlen(prefix) +
      strlen(FTS5Keyword) + 1 +
      strlen(FTS5Number) + 1 +
      strlen(FTS5Nodename) + 1 +
      strlen(FTS5Sysopname) + 1 +
      strlen(FTS5Location) + 1 +
      strlen(FTS5Phone) + 1 +
      strlen(FTS5Baud) + 1 +
      strlen(FTS5Flags) +
      strlen(postfix) + 1;

    linebuf = malloc(bufsize);
    if (strlen(FTS5Flags)) /* Entry has flags */
      {
        snprintf(linebuf, bufsize, "%s%s,%s,%s,%s,%s,%s,%s,%s%s", prefix,
            FTS5Keyword, FTS5Number, FTS5Nodename, FTS5Location,
            FTS5Sysopname, FTS5Phone, FTS5Baud, FTS5Flags, postfix);
      }
    else
      {
        snprintf(linebuf, bufsize, "%s%s,%s,%s,%s,%s,%s,%s%s", prefix,
            FTS5Keyword, FTS5Number, FTS5Nodename, FTS5Location,
            FTS5Sysopname, FTS5Phone, FTS5Baud, postfix);
      }
    if (crc != NULL)
        *crc = CRC16String(linebuf, *crc);
    fputs_result = fputs(linebuf, file);
    free(linebuf);
    return fputs_result;
}

int
CopyComment(FILE * output, char *Copyfile, const char *year,
            unsigned short *crc)
 /* Returns the number of lines written */
{
    const char *yearread;
    char *yearwrite;
    char yearcharbuf;
    char *linebegin;
    char *templine, *templn2;
    int lineno = 0;
    FILE *CopyFILE;
    char fullname[MYMAXDIR];
    char linebuf[linelength];

    /* Removed 12/12/12 */
    /* Don't include copyright, prolog and epilog unless we make the composite list */
    /* if (MakeType != LEVEL_TOP)
        return lineno; */
    if (!Copyfile)
        return lineno;
    myfnmerge(fullname, NULL, MasterDir, Copyfile, NULL);
    strlcpy(linebuf, ";S", sizeof linebuf);
    CopyFILE = fopen(fullname, "r");
    if (!CopyFILE)
    {
        if (MakeType == LEVEL_TOP)
        {
            /* In the composite nodelist, these files should be present. */
            mklog(LOG_ERROR, "WARNING -- file %s not found", Copyfile);
        }
        return lineno;
    }
    while ((linebegin =
            fgets(linebuf + 3, linelength - 3, CopyFILE)) != NULL)
    {
        if (linebegin[0] == '\x1a') /* CTRL-Z at beginning of line. */
        {
            return lineno;
        }
        if (RemoveBOM)
        {
            templine = strstr(linebegin, "\xef\xbb\xbf");
            if (templine != NULL) /* BOM found on line */
            {
                templn2 = templine + 3; /* Move rest of line over BOM */
                memmove(templine, templn2, strlen(templn2)+1);
            }
        }
        if (linebegin[0] != ';')
        {
            linebegin = linebuf;
            linebegin[2] = ' '; /* Kill the '\0' after ";S" */
        }
        if (linebegin[1] == 'E')
            linebegin[1] = 'S';
        if (year != NULL)
        {
            if ((yearwrite = strstr(linebegin, "####")) != NULL)
            {
                for (yearread = year, yearcharbuf = *(yearread++);
                     yearcharbuf; yearcharbuf = *(yearread++))
                    *(yearwrite++) = yearcharbuf;
            }
        }
        cutspaces(linebegin);
        strcat(linebegin, "\r\n");
        *crc = CRC16String(linebegin, *crc);
        if (fputs(linebegin, output) == EOF)
            die(254, "Write error on '%s'", OutFile);
        lineno++;
    }
    fclose(CopyFILE);
    return lineno;
}

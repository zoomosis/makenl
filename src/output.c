/* $Id: output.c,v 1.7 2005/07/18 20:30:58 mbroek Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "makenl.h"
#include "fts5.h"
#include "crc16.h"
#include "fileutil.h"
#include "mklog.h"

#ifdef MALLOC_DEBUG
#include "rmalloc.h"
#endif

#ifdef DMALLOC
#include "dmalloc.h"
#endif

char ErrorMessage[linelength];

int
OutputErrorLine(FILE * file, const char *pre, const char *wrongy,
                const char *post, unsigned short *crc)
{
    char *linebuf;
    int fputs_result;

    if (FTS5Keyword)            /* Do we have an Nodelist line to be
                                   printed also? */
        if (OutputFTS5Line(file, pre, post, crc) == EOF)
            return EOF;

    linebuf = malloc(strlen(pre) +
                     strlen(wrongy) + 4 +
                     strlen(ErrorMessage) + strlen(post) + 1);
    if (!linebuf)
        die(253, 1, "No memory left for error message buffer!\n");
    sprintf(linebuf, "%s%s -- %s%s", pre, wrongy, ErrorMessage, post);
    if (crc)
        *crc = CRC16String(linebuf, *crc);
    fputs_result = fputs(linebuf, file);
    mklog(0, "%s", linebuf);
    free(linebuf);
    return fputs_result;
}

int
OutputFTS5Line(FILE * file, const char *prefix, const char *postfix,
               unsigned short *crc)
{
    char *linebuf;
    int fputs_result;

    linebuf = malloc(strlen(prefix) +
                     strlen(FTS5Keyword) + 1 +
                     strlen(FTS5Number) + 1 +
                     strlen(FTS5Nodename) + 1 +
                     strlen(FTS5Sysopname) + 1 +
                     strlen(FTS5Location) + 1 +
                     strlen(FTS5Phone) + 1 +
                     strlen(FTS5Baud) + 1 +
                     strlen(FTS5Flags) + strlen(postfix) + 1);
    sprintf(linebuf, "%s%s,%s,%s,%s,%s,%s,%s,%s%s", prefix,
            FTS5Keyword, FTS5Number, FTS5Nodename, FTS5Location,
            FTS5Sysopname, FTS5Phone, FTS5Baud, FTS5Flags, postfix);
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
    int lineno = 0;
    FILE *CopyFILE;
    char fullname[MYMAXDIR];
    char linebuf[linelength];

    if (!Copyfile)
        return lineno;
    myfnmerge(fullname, NULL, MasterDir, Copyfile, NULL);
    strcpy(linebuf, ";S");
    CopyFILE = fopen(fullname, "r");
    if (!CopyFILE)
        return lineno;
    while ((linebegin =
            fgets(linebuf + 3, linelength - 3, CopyFILE)) != NULL)
    {
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
            die(254, 1, "Write error on \"%s\"", OutFile);
        lineno++;
    }
    fclose(CopyFILE);
    return lineno;
}

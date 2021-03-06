#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include "makenl.h"
#include "merge.h"
#include "fts5.h"
#include "msg.h"
#include "fileutil.h"
#include "mklog.h"
#include "strtool.h"

static FILE *MergeInFILE, *MergeOutFILE;
static int MergeListAddr[4];
static int MergeMakeAddr[4];
static char MergeLine[linelength];

FILE *PrepareMerge(void)
{
    char (*extptr)[MYMAXEXT];
    struct tm *tmptr;
    int matchparts = -1;
    time_t utime;
    char linebuf[linelength];
    int linelevel, linenum;

    if (MergeFilename[0] == 0)
        return 0;
    if (MyAddress[A_NET] == 0)
    {
        mklog(LOG_INFO, "WARNING - Your Net or Region number is unknown, Merge cancelled");
        return 0;
    }
    for (extptr = OldExtensions; extptr < OldExtensions + 7; extptr++)
    {
        swapext(MergeFilename, MergeFilename, *extptr);
        MergeInFILE = fopen(MergeFilename, "rb");
        if (MergeInFILE)
            break;
    }
    if (MergeInFILE)
    {
        swapext(MergeFilename, MergeFilename, "999");
        MergeOutFILE = fopen(MergeFilename, "wb");
        if (!MergeOutFILE)
            die(254, "$Unable to create '%s'", MergeFilename);
        if (fgets(linebuf, linelength, MergeInFILE) == NULL)
        {
            die(254, "$fgets(linebuf, linelength, MergeInFILE) failed");
        }
        time(&utime);
        tmptr = localtime(&utime);
        fprintf(MergeOutFILE,
                ";A Local NODELIST update for %s, %s %d, %d\r\n",
                DOWLongnames[tmptr->tm_wday],
                MonthLongnames[tmptr->tm_mon], tmptr->tm_mday,
                tmptr->tm_year + 1900);
    }
    if (!MergeInFILE)
    {
        swapext(MergeFilename, MergeFilename, NULL); /* Kill any extension */

        mklog(LOG_INFO, "Unable to find distribution file '%s' less than 7 weeks old.", MergeFilename);
        mklog(LOG_INFO, "Processing continues without merging");
        return 0;
    }
    MergeMakeAddr[A_ZONE] = MergeListAddr[A_ZONE] = MyAddress[A_ZONE];
    MergeMakeAddr[A_NET] = MergeListAddr[A_NET] = MyAddress[A_NET];
    MergeMakeAddr[Level4DPos[MakeType]] =
        MergeListAddr[Level4DPos[MakeType]] = MakeNum;
    if (fgets(linebuf, linelength, MergeInFILE) == NULL)
    {
        fclose(MergeInFILE);
        MergeInFILE = NULL;
        return MergeOutFILE;
    }
    do
    {
        if (linebuf[0] == '\x1A')
        {
            fclose(MergeInFILE);
            MergeInFILE = NULL;
            return MergeOutFILE;
        }
        strlcpy(MergeLine, linebuf, sizeof MergeLine);
        ParseFTS5(linebuf, &linelevel, &linenum);
        if (Level4DPos[linelevel] <= matchparts)
            return MergeOutFILE;
        if (Level4DPos[linelevel] <= Level4DPos[MakeType])
        {
            switch (Level4DPos[linelevel])
            {
            case A_NODE:
                MergeListAddr[A_NODE] = linenum;
                break;
            case A_ZONE:
                MergeListAddr[A_ZONE] = linenum;
                if (MergeMakeAddr[A_ZONE] == 0)
                {
                    MergeMakeAddr[A_ZONE] = linenum;
                    mklog(LOG_INFO, "WARNING - Your Zone is unknown, assuming Zone %d for merge",
                      MergeMakeAddr[A_ZONE]);
                }
                /* FALLTHROUGH */
            case A_NET:
                MergeListAddr[A_NET] = linenum;
                MergeListAddr[A_NODE] = 0;
            }
            while (matchparts < Level4DPos[linelevel] &&
                   MergeMakeAddr[matchparts + 1] ==
                   MergeListAddr[matchparts + 1])
                matchparts++;
            if (Level4DPos[MakeType] == matchparts)
            {
                /* Found the part to replace... */
                if (MakeType == LEVEL_NODE)
                {
                    if (fgets(MergeLine, linelength, MergeInFILE) == NULL)
                    {
                        die(254, "$fgets(MergeLine, linelength, MergeInFILE) failed");
                    }
                    return MergeOutFILE;
                }
                if (++linelevel <= MakeType)
                    return MergeOutFILE;
                while (fgets(linebuf, linelength, MergeInFILE) != NULL)
                {
                    strlcpy(MergeLine, linebuf, sizeof MergeLine);
                    ParseFTS5(linebuf, &linelevel, &linenum);
                    if (linelevel <= MakeType)
                        return MergeOutFILE;
                }
                break;
            }
        }
        fputs(MergeLine, MergeOutFILE);
    }
    while (fgets(linebuf, linelength, MergeInFILE) != NULL);
    fclose(MergeInFILE);
    MergeInFILE = NULL;
    return MergeOutFILE;
}

void FinishMerge(void)
{
    if (!MergeOutFILE)
        return;
    while (MergeInFILE)
    {
        fputs(MergeLine, MergeOutFILE);
        if (fgets(MergeLine, linelength, MergeInFILE) == NULL)
        {
            fclose(MergeInFILE);
            MergeInFILE = NULL;
        }
    }
    fclose(MergeOutFILE);
    MergeOutFILE = NULL;
}

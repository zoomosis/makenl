/* $Id: makenl.c,v 1.28 2004/09/08 18:47:51 mbroek Exp $ */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "makenl.h"
#include "config.h"
#include "fts5.h"
#include "lsttool.h"
#include "fileutil.h"
#include "merge.h"
#include "msg.h"
#include "proc.h"
#include "crc16.h"
#include "version.h"
#include "unused.h"
#include "mklog.h"

#ifdef MALLOC_DEBUG
#include "rmalloc.h"
#endif

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#if defined(__MSDOS__) && defined(__TURBOC__)
extern unsigned _stklen = 16384;
#endif

const char *const DOWLongnames[7] =
    { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
    "Saturday"
};
const char *const MonthLongnames[12] =
    { "January", "February", "March", "April",
    "May", "June", "July", "August", "September",
    "October", "November", "December"
};

int NewExtWDay = 6;

int MakeType = -1;
int MakeNum;

int ExitCode;
int JustTest;

int MakenlDebug = 0;            /* 1 - Debugs some code here... */

char *WorkFile = NULL;

static FILE *CFG_file;
static FILE *MakeSourceFILE;
static char YearBuf[6];
static char NewFile[MYMAXPATH];
static char OutExt[MYMAXEXT];
static char CfgFilenameBuf[MYMAXPATH];
static char HeaderLine[linelength];
static char SubAddrText[20];
int WorkMode;
static int OldWeeks;
static struct tm *SplitTimePtr;
static time_t UnixTime;
static FILE *OutFILE;
static FILE *CommentsFILE;
static FILE *SelfMsgFILE;
static FILE *mainMergeOut;
static char *CfgFile = "makenl.ctl";
static unsigned short OutCRC;

/* Looks for the last day with (dow == weekday) before now.
   The returned time is offset days apart from the found day.
   The wall-clock-time of the returned time equals the wall-clock-
   time of now. If the dayligth-saving-time-flag does not change,
   the returned time is a multiple of 24 hours away */
static time_t searchdow(int weekday, int offset, struct tm **timebuf)
{
    time_t temp;
    struct tm *thetime;
    int isdst;

    time(&temp);
    thetime = localtime(&temp); /* Get the time */
    isdst = thetime->tm_isdst;
    weekday = weekday - thetime->tm_wday; /* How many days are before the
                                             wanted day? */
    if (weekday > 0)            /* We don't want to look into future - */
        weekday -= 7;           /* - go one week back */
    temp += 3600L * 24 * (offset + weekday); /* Jump to the required day */
    thetime = localtime(&temp);
    temp += 3600 * (isdst - thetime->tm_isdst); /* Do daylight corrections 
                                                 */
    if (timebuf)
        *timebuf = localtime(&temp);
    return temp;
}

void die(int exitcode, int on_stderr, const char *format, ...)
{
    char buf[1024], exitstr[50];
    va_list arg;
    
    va_start(arg, format);
    vsprintf(buf, format, arg);
    va_end(arg);

    *exitstr = '\0';

    if (exitcode != 0)
    {
        sprintf(exitstr, "(rc=%d) ", exitcode);
    }

    fprintf(
      on_stderr ? stderr : stdout,
      "%s%s%s\n",
      exitcode == 0 ? "" : "ABORT -- ",
      exitstr,
      buf
    );

    mklog(on_stderr ? 0 : 1, "%s", buf);
    mklog(1, "MakeNL finished (rc=%d)", exitcode);

    exit(exitcode);
}

static void showversion(void)
{
    fprintf(stderr,
      "\nMakeNL " MAKENL_VERSION "  Compiled on " __DATE__ " " __TIME__ "\n\n" MAKENL_DEDICATION "\n");
}

int main(int argc, char *argv[])
{
    unused(argc);

    showversion();

    DoCmdLine(argv, &CfgFile);
    if (!getext(NULL, CfgFile))
    {
        swapext(CfgFilenameBuf, CfgFile, "ctl");
        CfgFile = CfgFilenameBuf;
    }
    CFG_file = fopen(CfgFile, "r");
    die_if_file(CFG_file, CfgFile, 0);
    WorkFile = strdup(CfgFile);
    os_filecanonify(WorkFile);
    os_getcwd(CurDir, MYMAXDIR - 1);
    os_filecanonify(CurDir);
    WorkMode = parsecfgfile(CFG_file);
    mklog(1, "Using %s in %s", CfgFile, CurDir);

    for (OldWeeks = 3; OldWeeks >= 0; OldWeeks--)
    {
        searchdow(NewExtWDay, -7 * OldWeeks + 6, &SplitTimePtr);
        sprintf(OldExtensions[OldWeeks], "%03d",
                SplitTimePtr->tm_yday + 1);
    }
    sprintf(YearBuf, "%d", 1900 + SplitTimePtr->tm_year);
    sprintf(HeaderLine,
            ";A %s Nodelist for %s, %s %d, %s -- Day number %s : ",
            Levels[MakeType], DOWLongnames[SplitTimePtr->tm_wday],
            MonthLongnames[SplitTimePtr->tm_mon], SplitTimePtr->tm_mday,
            YearBuf, OldExtensions[0]);
    time(&UnixTime);
    SplitTimePtr = localtime(&UnixTime);
    printf("Begin processing %s -- %d:%02d, %s, %s %d, %d\n\n", OutFile,
           SplitTimePtr->tm_hour, SplitTimePtr->tm_min,
           DOWLongnames[SplitTimePtr->tm_wday],
           MonthLongnames[SplitTimePtr->tm_mon], SplitTimePtr->tm_mday,
           SplitTimePtr->tm_year + 1900);
    mklog(1, "Begin processing %s -- %d:%02d, %s, %s %d, %d", OutFile,
	    SplitTimePtr->tm_hour, SplitTimePtr->tm_min,
	    DOWLongnames[SplitTimePtr->tm_wday],
	    MonthLongnames[SplitTimePtr->tm_mon], SplitTimePtr->tm_mday,
	    SplitTimePtr->tm_year + 1900);
    if (ShouldProcess)
    {
        myfnmerge(NewFile, NULL, OutDir, OutFile, NULL);
	mklog(4, "main: shouldprocess %s", NewFile);
        swapext(NewFile, NewFile, "$$$");
        OutFILE = fopen(NewFile, "wb");
        die_if_file(OutFILE, NewFile, 1);
        fprintf(OutFILE, "%s%05u\r\n", HeaderLine, OutCRC);
        CopyrightLines =
            CopyComment(OutFILE, CopyrightFile, YearBuf, &OutCRC);
        CopyComment(OutFILE, PrologFile, NULL, &OutCRC);
    }
    if (CommentsFile[0] != 0)
    {
        if (!filecmp(CommentsFile, "STDOUT"))
            CommentsFILE = stdout;
        else if (!filecmp(CommentsFile, "STDERR"))
            CommentsFILE = stderr;
        else
        {
            CommentsFILE = fopen(CommentsFile, "w");
            die_if_file(CommentsFILE, CommentsFile, 1);
        }
    }
    mainMergeOut = PrepareMerge();
    if (!JustTest && MailerFlags & MF_SELF)
    {
        /* That means: Do a mailing if errors occur */
        UsualMSGFlags = MF_DOIT << MF_SHIFT_ERRORS;
        SelfMsgFILE = OpenMSGFile(MyAddress, NULL);
    }
    if (WorkMode == CFG_DATA)
        ExitCode =
            processfile(MakeType, MakeNum, CFG_file, OutFILE, NULL,
                        mainMergeOut, SelfMsgFILE, &OutCRC, &WorkMode);
    else if (MakeSourceFile[0] != 0)
    {
        myfnmerge(CfgFilenameBuf, NULL, MasterDir, MakeSourceFile, NULL);
        MakeSourceFILE = fopen(CfgFilenameBuf, "r");
        die_if_file(MakeSourceFILE, CfgFilenameBuf, 0);
        ExitCode = processfile(MakeType, MakeNum, MakeSourceFILE, OutFILE, NULL,
          mainMergeOut, SelfMsgFILE, &OutCRC, &WorkMode);
        fclose(MakeSourceFILE);
    }
    SelfMsgFILE = CloseMSGFile(ExitCode);
    if (ExitCode > 1)
        die(255, 1, "Fatal error in %s", WorkFile);
    ProcessFILES(WorkMode, CFG_file, OutFILE, CommentsFILE, mainMergeOut,
                 &OutCRC);
    FinishMerge();
    if (ShouldProcess)
    {
        CopyComment(OutFILE, EpilogFile, NULL, &OutCRC);
        OutCRC = CRC16DoByte(0, CRC16DoByte(0, OutCRC));
        putc('\x1A', OutFILE);
        fseek(OutFILE, 0L, SEEK_SET);
        fprintf(OutFILE, "%s%05u\r\n", HeaderLine, OutCRC);
        fclose(OutFILE);
        WorkMode = 0;
        if (installlist(NewFile, OutExt) == 0) /* List changed */
        {
            char cmdbuf[1024];  /* space for CalledBatchfile */

            sprintf(cmdbuf, "%s %s" STR_DIRSEPARATOR "%s ",
                    CalledBatchFile, OutDir, OutFile);
            WorkMode = 0;       /* Why that?! see three lines above! */
            if (OutExt[0] == 0) /* If output is generic, we could diff and 
                                   ARC */
            {
                cleanold(OutDir, OutFile, OldExtensions[2]);
                WorkMode = makediff(NewFile);
                makearc(NewFile, 0);
                if (WorkMode & CAUSE_OUTDIFF)
                {
                    sprintf(cmdbuf + strlen(cmdbuf),
                            "%s" STR_DIRSEPARATOR "%s ", OutDir, OutDiff);
                    myfnmerge(CfgFilenameBuf, NULL, OutDir, OutDiff,
                              OldExtensions[0]);
                    makearc(CfgFilenameBuf, 1);
                    if (WorkMode & CAUSE_THRESHOLD)
                    {
                        myfnmerge(CfgFilenameBuf, NULL, OutDir, OutFile,
                                  OldExtensions[0]);
                        makearc(CfgFilenameBuf, 0);
                    }
                }
                else
                    strcat(cmdbuf, "no-diff ");
            }
            else
	    {
                strcat(cmdbuf, "no-diff ");
		/*
		 * New feature: compress hub and host segments.
		 * Added in 2004 when file size doesn't matter anymore.
		 */
		myfnmerge(CfgFilenameBuf, NULL, OutDir, OutFile, NULL);
		makearc(CfgFilenameBuf, 1);
		strcpy(NewFile, CfgFilenameBuf);
		mklog(4, "main: NewFile \"%s\"", MAKE_SS(NewFile));
	    }

            sprintf(cmdbuf + strlen(cmdbuf), "%c %c %c %c %c %c\n",
                    OldExtensions[0][0], OldExtensions[0][1],
                    OldExtensions[0][2], OldExtensions[1][0],
                    OldExtensions[1][1], OldExtensions[1][2]);
            if (SubmitFile && BatchFile[0])
            {
                FILE *fp = fopen(BatchFile, "w");

                if (fp)
                {
                    fputs(os_deslashify(cmdbuf), fp);
                    fclose(fp);
                }
            }

            os_filecanonify(NewFile);
            if (MailerFlags & MF_SUBMIT && SubmitFile
                && OpenMSGFile(SubmitAddress, NewFile))
            {
                if (MyAddress[A_ZONE] == SubmitAddress[A_ZONE])
                    sprintf(SubAddrText, "%d/%d", SubmitAddress[A_NET],
                            SubmitAddress[A_NODE]);
                else
                    sprintf(SubAddrText, "%d:%d/%d", SubmitAddress[A_ZONE],
                            SubmitAddress[A_NET], SubmitAddress[A_NODE]);
                fprintf(stdout, "\nSending \"%s\" to %s\n", NewFile,
                        SubAddrText);
		mklog(1, "Sending \"%s\" to %s", NewFile, SubAddrText);
            }
        }
        cleanit();
    }
    else
        ExitCode += 3;
    fprintf(stdout, "\nCRC = %05u\n\n", OutCRC);

    mklog(1, "CRC = %05u", OutCRC);
    mklog(1, "MakeNL finished (rc=%d)", ExitCode);

    return ExitCode;
}

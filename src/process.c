/* $Id: process.c,v 1.8 2013/09/03 22:51:36 ozzmosis Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "makenl.h"
#include "crc16.h"
#include "proc.h"
#include "msg.h"
#include "fts5.h"
#include "lsttool.h"
#include "fileutil.h"
#include "upcont.h"
#include "stack.h"
#include "config.h"
#include "mklog.h"

#ifdef MALLOC_DEBUG
#include "rmalloc.h"
#endif

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int ShouldProcess = USUAL_PROCESSING;
int SubmitFile;

static int LogError(int now, int prev, FILE * OutFILE, FILE * SelfMsgFILE,
                    unsigned short *crc);

void
ProcessFILES(int WorkMode, FILE * CfgFILE, FILE * OutFILE,
             FILE * CommentsFILE, FILE * MergeOutFILE, unsigned short *crc)
{
    char *tmpptr;
    int fieldno;
    int searchwhere;
    int mustbenew;
    FILE *commFILE;
    int subfile_level;
    int num;
    FILE *listFILE;
    FILE *NotifyMsgFILE = 0;
    int processstatus = 0;
    char command[8];
    char filename[MYMAXFILE + MYMAXEXT];
    char notifyaddr[18];
    char foundfile[MYMAXDIR];
    char linebuf[linelength];
    const char *const *subleveltxt;

    /* another function may have already closed the file... */

    if (feof(CfgFILE))
    {
        return;
    }

    while (fgets(linebuf, linelength, CfgFILE) != NULL)
    {
        UsualMSGFlags = MailerFlags & (MF_ERRORS | MF_RECEIPT);
        tmpptr = strchr(linebuf, ';');
        if (tmpptr)
            *tmpptr = 0;
        if (*cutspaces(linebuf) == 0)
            continue;
        fieldno =
            sscanf(linebuf, "%7s %d %12s %17s", command, &num, filename,
                   notifyaddr);
        strupr(command);
        if (fieldno < 3)        /* "Net 2410 NET_2410" is minimum... */
            die(255, "%s\n -- Too few parameters -- Statement ignored\n", linebuf);
        subfile_level = xlate_switch(command, MakeTypes);
        if (MakeType >= subfile_level)
            die(255, "%s\n -- '%s' is invalid list type -- Statement ignored\n",
                linebuf, command);
        if (subfile_level < LEVEL_HOST)
        {
            NotifyAddress[A_NET] = num;
            NotifyAddress[A_NODE] = 0;
        }
        else if (MakeType <= LEVEL_HOST)
        {
            NotifyAddress[A_NET] = MakeNum;
            NotifyAddress[A_NODE] = num;
        }
        else
            UsualMSGFlags = 0;
        if (fieldno >= 4)
        {
            if (ParseAddress(notifyaddr, NotifyAddress) != 0)
                die(255, "Invalid NOTIFY address '%s' ignored\n", notifyaddr);
            UsualMSGFlags = MailerFlags & (MF_ERRORS | MF_RECEIPT);
        }
        else
            NotifyAddress[A_ZONE] = MyAddress[A_ZONE];
        if (num < 0)
            die(255, "%s\n -- \"%d\" is invalid list number -- Statement ignored\n",
                linebuf, num);
        subleveltxt = LevelsSimple + subfile_level;
        searchwhere = 0;
        foundfile[0] = '\0';
        do
        {
            if (ShouldProcess == 0 && MergeOutFILE == 0) /* Process only
                                                            new files */
                mustbenew = 1;
            else
                mustbenew = 0;
            searchwhere =
                openlist(&listFILE, filename, foundfile, searchwhere,
                         mustbenew);
            os_filecanonify(filename);
            os_filecanonify(foundfile);
            if (searchwhere > 0)
            {
                mklog(LOG_INFO, "Processing %-8s%5d -- file '%s'", *subleveltxt, num, foundfile);
                if (UsualMSGFlags != 0 && searchwhere < SEARCH_UPDATE + 1) 
                {
                    /* newly received file */
                    NotifyMsgFILE = OpenMSGFile(NotifyAddress, NULL);
                    if (NotifyMsgFILE)
                    {
                        fprintf(NotifyMsgFILE, "Your nodelist update '%s' has been received", WorkFile);
                    }
                }
                commFILE =
                    (searchwhere ==
                     SEARCH_MASTER + 1) ? NULL : CommentsFILE;
                processstatus =
                    processfile(subfile_level, num, listFILE, OutFILE,
                                commFILE, MergeOutFILE, NotifyMsgFILE, crc,
                                &WorkMode);

                fclose(listFILE);

                if (processstatus != 2) /* No fatal error */
                {
                    if (processstatus > ExitCode)
                        ExitCode = processstatus;
                    break;
                }
                ExitCode = processstatus;
                NotifyMsgFILE = CloseMSGFile(ExitCode);
                if (BadDir[0] != 0)
                    CopyOrMove(0, foundfile, BadDir, WorkFile);
                else
                    unlink(foundfile);
            }
        }
        while (searchwhere > 0);
        switch (searchwhere)
        {
        case 0:                /* not found */
            if (!ShouldProcess)
                break;
            mklog(LOG_INFO, "No file found for %s %d file '%s'", LevelsSimple[subfile_level], num, filename);
            break;
        case SEARCH_UPLOAD + 1:
            cleanold(MailfileDir, filename, NULL);
            /* FALLTHROUGH */
        case SEARCH_MAILFILE + 1:
            CloseMSGFile(processstatus);
            cleanold(UpdateDir, filename, NULL);
            if (!ShouldProcess)
            {
                CopyOrMove(0, foundfile, UpdateDir, filename);
                break;
            }
            CopyOrMove(0, foundfile, MasterDir, filename);
            break;
        case SEARCH_MASTER + 1:
            if (!ShouldProcess)
                break;
            if (getext(NULL, filename)) /* explicit name */
                break;
            myfnmerge(linebuf, NULL, MasterDir, filename,
                      OldExtensions[0]);
            if (filecmp(foundfile, linebuf))
                rename(foundfile, linebuf); /* new extension */
            cleanold(MasterDir, filename, OldExtensions[2]);
            break;
        case SEARCH_UPDATE + 1:
            if (!ShouldProcess)
                break;
            cleanold(MasterDir, filename, OldExtensions[2]);
            CopyOrMove(0, foundfile, MasterDir, filename);
        }
    }

    fclose(CfgFILE);
}

int
processfile(int myMakeType, int myMakeNum, FILE * InputFILE,
            FILE * OutFILE, FILE * FooFILE, FILE * MergeOutFILE,
            FILE * SelfMsgFILE, unsigned short *OutCRC, int *WorkMode)
{
    long outpos = 0;
    long mergeoutpos = 0;
    int error;
    int level;
    int num;
    int contextlevel;
    int contextnum;
    int totalerror = 0;
    unsigned short oldcrc;
    static char InputLine[linelength];

    oldcrc = *OutCRC;
    if (OutFILE)
    {
        outpos = ftell(OutFILE);
        if (myMakeType < LEVEL_HUB)
        {
            *OutCRC = CRC16String(";\r\n", *OutCRC);
            fputs(";\r\n", OutFILE);
        }
    }
    if (MergeOutFILE)
        mergeoutpos = ftell(MergeOutFILE);
    if (FooFILE)
        fprintf(FooFILE, "\nComments from %s:\n\n", WorkFile);
    while ((fgets(InputLine, linelength, InputFILE) != NULL)
           && (InputLine[0] != '\032'))
    {
        error = ParseFTS5(InputLine, &level, &num);
        if (level >= LEVEL_COMMENT)
        {
            if (level == LEVEL_COMMENT && FooFILE && InputLine[1] != 0)
            {
                fputs(InputLine, FooFILE);
                putc('\n', FooFILE);
            }
        }
        else
        {
            if (addnumber(myMakeType, myMakeNum, 1) != 0)
                error = 2;
            contextlevel = level; /* Initialize the context level */
            if (!error)
                error =
                    UpdateContext(level, num, myMakeNum, &contextnum,
                                  &contextlevel, myMakeType);
            if (error)
            {
                contextlevel = myMakeType;
                totalerror = LogError(2, 0, OutFILE, SelfMsgFILE, OutCRC);
            }
            if (OutFILE)
                OutputFTS5Line(OutFILE, "", "\r\n", OutCRC);
            if (MergeOutFILE)
                OutputFTS5Line(MergeOutFILE, "", "\r\n", NULL);
            break;
        }
    }
    while ((fgets(InputLine, linelength, InputFILE) != NULL)
            && (InputLine[0] != '\032'))
    {
        error = ParseFTS5(InputLine, &level, &num);
        if (error && *WorkMode == CFG_DATA
            && !strncmp(FTS5Keyword, "FILES", 5))
        {
            *WorkMode = error = CFG_FILES;
            break;
        }
        if (level < LEVEL_COMMENT)
        {
            if (!error)
                error =
                    UpdateContext(level, num, 0, &contextnum,
                                  &contextlevel, myMakeType);
            if (!error)
                error = addnumber(level, num, 0);
            if (error)
            {
                totalerror =
                    LogError(error, totalerror, OutFILE, SelfMsgFILE,
                             OutCRC);
                continue;
            }
            if (OutFILE)
            {
                if (level < LEVEL_HUB)
                    /* Make it more readable - empty lines before NET,
                       REGION and ZONE */
                {
                    *OutCRC = CRC16String(";\r\n", *OutCRC);
                    fputs(";\r\n", OutFILE);
                }
                OutputFTS5Line(OutFILE, "", "\r\n", OutCRC);
            }
            if (MergeOutFILE)
            {
                if (level < LEVEL_HUB)
                    /* Make it more readable - see above */
                    fputs(";\r\n", MergeOutFILE);
                OutputFTS5Line(MergeOutFILE, "", "\r\n", NULL);
            }
        }
        else if (level == LEVEL_COMMENT && InputLine[1] != 0)
        {
            if (FooFILE)
            {
                fputs(InputLine, FooFILE);
                putc('\n', FooFILE);
            }
            if (InputLine[1] == 'E' && OutFILE) /* Pass through only
                                                   errors */
            {
                strcat(InputLine, "\r\n");
                *OutCRC = CRC16String(InputLine, *OutCRC);
                fputs(InputLine, OutFILE);
            }
        }
    }
    if (totalerror == 2)        /* 2 means fatal */
    {
        unmarkstack();          /* Throw away any numbers of this file */
        sprintf(InputLine,
                "Fatal error(s) caused file '%s' to be rejected\r\n",
                WorkFile);
        fputs(InputLine, stdout);
        if (SelfMsgFILE)
            fputs(InputLine, SelfMsgFILE);
        *OutCRC = oldcrc;
        if (OutFILE)
            fseek(OutFILE, outpos, SEEK_SET);
        if (MergeOutFILE)
            fseek(MergeOutFILE, mergeoutpos, SEEK_SET);
    }
    else
    {
        if (OutFILE)
            SubmitFile = 1;
        if (MergeOutFILE && myMakeType < LEVEL_HUB)
            fputs(";\r\n", MergeOutFILE);
    }

    if (SelfMsgFILE && totalerror == 0)
    {
        fputs(" and processed without error.\r\n", SelfMsgFILE);
    }
    return totalerror;
}

static int
LogError(int now, int prev, FILE * OutFILE, FILE * SelfMsgFILE,
         unsigned short *crc)
{
    OutputErrorLine(stdout, "", WorkFile, "\n", NULL);
    if (OutFILE)
        OutputErrorLine(OutFILE, ";E ", "", "\r\n", crc);
    if (SelfMsgFILE)
    {
        if (prev == 0)          /* First Error */
            fputs
                (".  Please correct the following errors and resubmit.\r\n\r\n",
                 SelfMsgFILE);
        OutputErrorLine(SelfMsgFILE, "", "", "\r\n", NULL);
        fputs("\r\n", SelfMsgFILE);
    }
    return max(now, prev);
}

/* $Id: config.c,v 1.28 2016/10/29 06:58:12 ajleary Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "makenl.h"
#include "msg.h"
#include "config.h"
#include "fts5.h"
#include "procfile.h"
#include "lsttool.h"
#include "fileutil.h"
#include "mklog.h"
#include "version.h"
#include "strtool.h"
#include "snprintf.h"

#define MF_EVERYWHERE(x) ((x) | ((x) << MF_SHIFT_ERRORS) | ((x) << MF_SHIFT_SUBMIT))
#define MAIL_CRASH MF_EVERYWHERE(MF_CRASH)
#define MAIL_HOLD MF_EVERYWHERE(MF_HOLD)
#define MAIL_INTL MF_EVERYWHERE(MF_INTL)


int ArcCopySet = 0;
int ArcMoveSet = 0;

extern int nl_baudrate[];

static int CheckErrors(int mode) /* mode is -1 or 0 or CFG_DATA or
                                    CFG_FILES */
{
    if (mode != -1)             /* This means there already is an error */
    {
        if (MakeType == -1)
        {
            mklog(LOG_ERROR, "No MAKE specified -- nothing to make");
            mode = -1;
        }
        else
        {
            if (mode == 0 && MakeSourceFile[0] == 0) /* No Source
                                                        specified */
            {
                mklog(LOG_ERROR, "No input file or data specified");
                mode = -1;
            }
            else if (mode == CFG_DATA && MakeSourceFile[0] != 0)
            {
                mklog(LOG_ERROR, "Master input file and DATA stream both specified");
                mode = -1;
            }
            else if (OutFile[0] == 0)
            {
                mklog(LOG_ERROR, "No output file specified");
                mode = -1;
            }
            else
            {
                if (MasterDir[0] == 0)
                {
                    mklog(LOG_ERROR, "No directory for master files specified -- using '%s'",
                            CurDir);
                    strlcpy(MasterDir, CurDir, sizeof MasterDir);
                }
                if (OutDir[0] == 0)
                {
                    mklog(LOG_ERROR, "No directory for output file specified -- using '%s'",
                            MasterDir);
                    strlcpy(OutDir, MasterDir, sizeof OutDir);
                }
                if (UploadDir[0] != 0 && !strcmp(UploadDir, MasterDir))
                {
                    mklog(LOG_ERROR, "UPLoads and MASter both specify '%s'", MasterDir);
                    mode = -1;
                }
                else if (MailfileDir[0] != 0
                         && !strcmp(MailfileDir, MasterDir))
                {
                    mklog(LOG_ERROR, "MAIlfiles and MASter both specify '%s'", MasterDir);
                    mode = -1;
                }
                else if (UpdateDir[0] != 0
                         && !strcmp(UpdateDir, MasterDir))
                {
                    mklog(LOG_ERROR, "UPDate and MASter both specify '%s'", MasterDir);
                    mode = -1;
                }
                else if (UpdateDir[0] != 0 && UploadDir[0] != 0
                         && !strcmp(UpdateDir, UploadDir))
                {
                    mklog(LOG_ERROR, "UPLoads and UPDate both specify '%s'", UpdateDir);
                    mode = -1;
                }
                else if (UpdateDir[0] != 0 && MailfileDir[0] != 0
                         && !filecmp(UpdateDir, MailfileDir))
                {
                    mklog(LOG_ERROR,  "UPDate and MAIlfiles both specify '%s'", UpdateDir);
                    mode = -1;
                }
                else if (UploadDir[0] != 0 && MailfileDir[0] != 0
                         && !filecmp(UploadDir, MailfileDir))
                {
                    mklog(LOG_ERROR, "UPLoads and MAIlfiles both specify '%s'", UploadDir);
                    mode = -1;
                }
            }
        }
        if (MakeType == 0 && Levels[LEVEL_TOP][0] == 0)
        {
            mklog(LOG_ERROR,  "WARNING -- No net NAME given for list header (use /N or NAME verb)");
        }
        if (PrivateLevel > 0 && PrivateLevel < MakeType)
        {
            mklog(LOG_ERROR, "PRIvate phone numbers referred to too high a level");
            mode = -1;
        }
        if (MyAddress[A_NET] == 0
            && (MakeType == LEVEL_REGION || MakeType == LEVEL_HOST))
            MyAddress[A_NET] = MakeNum;
        if (MyAddress[A_ZONE] == 0 && MakeType == LEVEL_ZONE)
            MyAddress[A_ZONE] = MakeNum;
        if (MyAddress[A_NODE] == 0 && MakeType >= LEVEL_HUB)
            MyAddress[A_NODE] = MakeNum;
        if (mode != -1 && MailerFlags != 0)
        {
            if (MessageDir[0] == 0)
            {
                mklog(LOG_ERROR, "No message directory specified for notification");
                mode = -1;
            }
            else if (MyAddress[A_NET] == 0)
            {
                mklog(LOG_ERROR, "No local 'From' net address for notification messages.");
                mode = -1;
            }
            else if (SubmitAddress[A_NET] != 0
                     && SubmitAddress[A_ZONE] == 0)
                SubmitAddress[A_ZONE] = MyAddress[A_ZONE];
            if (MailerFlags & MAIL_INTL && MyAddress[A_ZONE] == 0)
            {
                mklog(LOG_ERROR, "Forced 'INTL' kludge requested, but your zone is unknown.");
                mode = -1;
            }
        }
        if ((ArcCopySet == 0) && (ArcMoveSet == 1))
        {
            mklog(LOG_ERROR, "'ArcMove' keyword found but no 'ArcCopy' keyword.");
            mode = -1;
        }
        if ((ArcCopySet == 1) && (ArcMoveSet == 0))
        {
            mklog(LOG_ERROR, "'ArcCopy' keyword found but no 'ArcMove' keyword.");
            mode = -1;
        }
    }
    return mode;
}

static int
getswitch(char *argument, const struct switchstruct *swstrings,
          char **value)
{
    if (argument[0] != '-')
    {
        *value = argument;
        return -1;
    }
    if ((*value = strchr(argument++, '=')) != NULL)
    {
        (*value)[0] = 0;        /* Kill the '=' sign */
        (*value)++;             /* Point to the value */
    }
    return xlate_switch(strupper(argument), swstrings);
}

unsigned int
xlate_switch(const char *swit, const struct switchstruct *desc)
{
    const char *swptr, *nameptr;
    int cnt;

    while (desc->name)
    {
        swptr = swit;
        nameptr = desc->name;
        cnt = 0;
        while (*swptr && *swptr == *nameptr)
        {
            cnt++;
            swptr++, nameptr++;
        }
        if (!*swptr &&  (cnt >= desc->minlen))
            break;              /* Found the switch in the list */
        if (swptr - swit > desc->minlen)
            break;
        desc++;
    }
    return desc->value;
}

static const struct switchstruct SwitchXLate[] = {
    {"MERGE", 1, 'M'},
    {"NAME", 1, 'N'},
    {"PROCESS", 1, 'P'},
    {"TEST", 1, 'T'},
    {"ZONE", 1, 'Z'},
    {"DEBUG", 1, 'D'},
    {"CREDITS", 1, 'C'},
    {NULL, 0, 0}
};

static void show_usage(void)
{
    printf(
      "Usage: makenl [<config file>] [-p|-t] [-m[=<nodelist>]] [-n=<netname>] [-d]" "\n"
      "\n"
      "  -p (-process)  Operate in Process mode, overriding Process statement" "\n"
      "                 in makenl.ctl." "\n"
      "\n"
      "  -t (-test)     Operate in Test mode even on the normal process day." "\n"
      "\n"
      "  -m (-merge)    Merge your updated nodelist segment with an existing" "\n"
      "                 distribution nodelist to produce a temporary nodelist" "\n"
      "                 containing your updates." "\n"
      "\n"
      "  -n (-name)     Specialized cosmetic switch. When you make a composite" "\n"
      "                 nodelist, the generated list begins, by default, with the" "\n"
      "                 line 'Nodelist for ...'. The -name switch may be used to" "\n"
      "                 insert a network name in front of the first word in that" "\n"
      "                 line. If the specified replacement name contains spaces," "\n"
      "                 the entire parameter must be enclosed in quotes." "\n"
      "\n"
      "  -d (-debug)    Enable debugging output for some functions." "\n"
      "\n"
      "  -c (-credits)  Who made this possible." "\n"
    );
    exit(255);
}

static void show_credits(void)
{
    printf(MAKENL_CREDITS);
    exit(255);
}

void DoCmdLine(char **argv, char **cfgfilename)
{
    char *tmpptr, *valueptr;

    while (*(++argv) != NULL)
    {
        switch (getswitch(*argv, SwitchXLate, &valueptr))
        {
        default:
            show_usage();
            break;

        case 'C':
            show_credits();
            break;

        case -1:
            *cfgfilename = valueptr;
            break;

        case 'M':
            /* Is it "-m" or "-m=BLA.LST"? */

            if (valueptr != NULL && *valueptr)
            {
                tmpptr = valueptr;
            }
            else
            {
                tmpptr = "nodelist";
            }
            strlcpy(MergeFilename, tmpptr, sizeof MergeFilename);
            break;

        case 'N':
            if (valueptr != NULL && *valueptr)
            {
                Levels[0] = valueptr;
            }
            break;

        case 'P':
            ShouldProcess = USUAL_PROCESSING | FORCED_PROCESSING;
            break;

        case 'T':
            ShouldProcess = 0;
            JustTest = 1;
            break;

        case 'Z':
            Levels[2] = Levels[1];
            break;

        case 'D':
            debug_mode = 1;
            break;
        }
    }
}

static char const cfgspacechars[] = " \t";

const struct switchstruct MailFlags[] = {
    {"CRASH", 5, MAIL_CRASH},
    {"HOLD", 4, MAIL_HOLD},
    {"INTL", 4, MAIL_INTL},
    {NULL, 0, -1}
};

#define NOTIFY_ERRORS 0xF0
#define NOTIFY_RECEIPT 0x0F
#define NOTIFY_ALL 0xFF
#define NOTIFY_SELF 4096

const struct switchstruct NotifyType[] = {
    {"ERRORS", 3, NOTIFY_ERRORS},
    {"RECEIPT", 3, NOTIFY_RECEIPT},
    {"ALL", 3, NOTIFY_ALL},
    {"SELF", 4, NOTIFY_SELF},
    {NULL, 0, -1}
};

const struct switchstruct MakeTypes[] = {
    {"MASTER", 3, LEVEL_TOP},
    {"COMPOSITE", 3, LEVEL_TOP},
    {"ZONE", 3, LEVEL_ZONE},
    {"REGION", 3, LEVEL_REGION},
    {"NETWORK", 3, LEVEL_HOST},
    {"HUB", 3, LEVEL_HUB},
    {"NODE", 3, LEVEL_NODE},
    {NULL, 0, -1}
};

/* Stupid mistake by BF: the three new verbs must be
   at least 4 characters long to be unique */

const struct switchstruct CfgEntries[] = {
    {"ARCCOPY", 4, CFG_ARCCOPY},
    {"ARCMOVE", 4, CFG_ARCMOVE},
    {"ARCOPEN", 4, CFG_ARCOPEN},
    {"BADFILES", 3, CFG_BADFILES},
    {"BATCHFILE", 3, CFG_BATCHFILE},
    {"BAUDRATE", 3, CFG_BAUDRATE},
    {"CALLEDBATCHFILE", 3, CFG_CALLEDBATCHFILE},
    {"CLEANUP", 3, CFG_CLEANUP},
    {"COMMENTS", 3, CFG_COMMENTS},
    {"COPYRIGHT", 3, CFG_COPYRIGHT},
    {"DATA", 4, CFG_DATA},
    {"EPILOG", 3, CFG_EPILOG},
    {"FILES", 5, CFG_FILES},
    {"FORCESUBMIT", 3, CFG_FORCESUBMIT},
    {"MAILFILES", 3, CFG_MAILFILES},
    {"MAKE", 3, CFG_MAKE},
    {"MASTER", 3, CFG_MASTER},
    {"MERGE", 3, CFG_MERGE},
    {"MESSAGES", 3, CFG_MESSAGES},
    {"MINPHONE", 3, CFG_MINPHONE},
    {"NAME", 3, CFG_NAME},
    {"NETADDRESS", 3, CFG_NETADDRESS},
    {"NOTIFY", 3, CFG_NOTIFY},
    {"OUTDIFF", 4, CFG_OUTDIFF},
    {"OUTFILE", 4, CFG_OUTFILE},
    {"OUTPATH", 4, CFG_OUTPATH},
    {"POINTS", 3, CFG_POINTS},
    {"PRIVATE", 3, CFG_PRIVATE},
    {"PROCESS", 4, CFG_PROCESS},
    {"PROLOG", 4, CFG_PROLOG},
    {"PUBLISH", 3, CFG_PUBLISH},
    {"SEND", 3, CFG_SUBMIT},
    {"SUBMIT", 3, CFG_SUBMIT},  /* SENd is an alias for SUBmit */
    {"THRESHOLD", 3, CFG_THRESHOLD},
    {"UPDATE", 3, CFG_UPDATE},
    {"UPLOADS", 3, CFG_UPLOADS},
    {"ALPHAPHONE", 4, CFG_ALPHAPHONE},
    {"ALLOWUNPUB", 6, CFG_ALLOWUNPUB},
    {"LOGFILE", 4, CFG_LOGFILE},
    {"LOGLEVEL", 4, CFG_LOGLEVEL},
    {"ALLOW8BIT", 6, CFG_ALLOW8BIT},
    {"REMOVEBOM", 3, CFG_REMOVEBOM},
    {NULL, 0, -1}
};

static const struct switchstruct DOWSwitchTab[] = {
    {"SUNDAY", 3, 0},
    {"MONDAY", 3, 1},
    {"TUESDAY", 3, 2},
    {"WEDNESDAY", 3, 3},
    {"THURSDAY", 3, 4},
    {"FRIDAY", 3, 5},
    {"SATURDAY", 3, 6},
    {"TODAY", 3, 7},
    {"TOMORROW", 3, 8},
    {NULL, 0, -1}
};

static const struct switchstruct PrivateDisp[] = {
    {"NONE", 1, -2},
    {"OK", 1, 0},
    {"ZONE", 1, LEVEL_ZONE},
    {"REGION", 1, LEVEL_REGION},
    {"HOST", 2, LEVEL_HOST},
    {"HUB", 2, LEVEL_HUB},
    {NULL, 0, -1}
};

static const struct switchstruct PointDisp[] = {
    {"NONE", 3, -2},            /* not allowed */
    {"HIDE", 1, -3},            /* kill any given phones */
    {"OK", 1, 0},               /* allowed, don't change phones */
    {"NODE", 3, LEVEL_NODE},    /* allowed, use node's phone */
    {NULL, 0, -1}
};

/* file - explicit filename
   gfile - generic filename
   [g]file - generic or explicit filename
   pfile - filename with optional path
   pgfile - generic filename with optional path
 */
/* *INDENT-OFF* */
/* Indent tears the array lines :-( */
struct
{
  int min, max;
} arglimit[] = {
  {4, 6},                       /* ARCCopy e.g. "Z pkzip -exo" */
  {2, 2},                       /* BADfiles path */
  {2, 2},                       /* BAUdrate a,b,c,d,e # without spaces!! */
  {1, 1},                       /* CLEanup */
  {2, 2},                       /* COMments pfile */
  {2, 2},                       /* COPyright file */
  {1, 1},                       /* DATA */
  {2, 2},                       /* EPIlog file */
  {1, 1},                       /* FILES */
  {2, 2},                       /* MAIlfiles path */
  {2, 4},                       /* MAKe type [ftnaddr [gfile]] */
  {2, 2},                       /* MASter path */
  {1, 2},                       /* MERge [pgfile] */
  {2, 2},                       /* MESsages path */
  {2, 2},                       /* MINphone parts */
  {2, 2},                       /* NAMe name */
  {2, 2},                       /* NETaddress ftnaddr */
  {2, 5},                       /* NOTify {ERRors|RECeipt|ALL|SELF} [CRASH] [HOLD] [INTL] */
  {2, 2},                       /* OUTDiff gfile */
  {2, 2},                       /* OUTFile [g]file */
  {2, 2},                       /* OUTPath path */
  {2, 2},                       /* PRIvate disp */
  {2, 2},                       /* PROCess day */
  {2, 2},                       /* PROLog file */
  {2, 2},                       /* PUBlish day */
  {2, 5},                       /* {SENd|SUBmit} ftnaddr [CRASH] [HOLD] [INTL] */
  {2, 3},                       /* {THReshold} arc [diff] */
  {2, 2},                       /* UPDate path */
  {2, 2},                       /* UPLoads path */
  {2, 2},                       /* POInt disp */
  {2, 2},                       /* BATCHFILE pfile */
  {2, 2},                       /* CALLEDBATCHFILE pfile */
  {4, 6},                       /* ARCMove e.g. "Z pkzip -exom" */
  {4, 6},                       /* ARCOpen e.g. "Z pkunzip -o" */
  {2, 2},                       /* ALPHaphone 1 or 0 - default 0 */
  {2, 2},                       /* ALLOwunpub 1 or 0 - default 0 */
  {2, 2},                        /* LOGFile pfile */
  {2, 2},                        /* LOGLevel 1..4 - default 1 */
  {2, 2},                        /* FORcesubmit 1 or 0 - default 0 */
  {2, 2},                        /* ALLOW8BIT 1 or 0 - default 0 */
  {2, 2}			 /* REMOVEBOM 1 or 0 - default 0 */
};
/* *INDENT-ON* */


int parsecfgfile(FILE * CFG)
{
    struct tm *mytime;
    int process_day;
    int notifybits;
    char *command;
    char **flagptr;
    int flagnum;
    int argcounter;
    int switchno;
    char *workptr;
    char foo[2];
    int mode;
    time_t thetime;
    char *args[5];
    char cfgline[linelength];
    char cfgSplit[linelength];
    int ArcOpenSet = 0;
    int i, j;
    char *p;

    /* Init valid baudrates */
    for (i = 0; i < MAX_BAUDRATES; i++)
        nl_baudrate[i] = 0;
    nl_baudrate[0] = 300;
    nl_baudrate[1] = 1200;
    nl_baudrate[2] = 2400;
    nl_baudrate[3] = 4800;
    nl_baudrate[4] = 9600;

    mode = 0;
    while (fgets(cfgline, linelength - 1, CFG) != NULL)
    {
        workptr = strchr(cfgline, ';');
        if (workptr)
            *workptr = 0;
        cutspaces(cfgline);
        strlcpy(cfgSplit, cfgline, sizeof cfgSplit);
        command = strtok(cfgSplit, cfgspacechars); /* Space and Tab */
        if (!command)
            continue;
        switchno = xlate_switch(strupper(command), CfgEntries);
        if (switchno == -1)
        {
            mklog(LOG_ERROR, "'%s': Unknown keyword '%s'", cfgline, command);
            mode = -1;
            break;
        }
        argcounter = 1;
        while (argcounter < arglimit[switchno].max)
        {
            args[argcounter - 1] = strtok(NULL, cfgspacechars);
            if (!args[argcounter - 1])
                break;
            argcounter++;
        }
        if (arglimit[switchno].min > argcounter)
        {
            mklog(LOG_ERROR, "Not enough parameters for '%s' command", cfgline);
            mode = -1;
            continue;
        }
        if (argcounter == arglimit[switchno].max && strtok(NULL, cfgspacechars)) /* Still 
                                                                                    more 
                                                                                    arguments... 
                                                                                  */
        {
            mklog(LOG_ERROR, "Too many parameters for '%s' command", cfgline);
            mode = -1;
            continue;
        }
        switch (switchno)
        {
        case CFG_DATA:
        case CFG_FILES:
            mode |= switchno;
            goto outofwhile;
        case CFG_ARCCOPY:
            ArcCopyExt[0] = args[0][0];
            if (argcounter == 6)
                snprintf(ArcCopyCmd, sizeof ArcCopyCmd, "%s %s %s %s", args[1], args[2], args[3], args[4]);
            else if (argcounter == 5)
                snprintf(ArcCopyCmd, sizeof ArcCopyCmd, "%s %s %s", args[1], args[2], args[3]);
            else
                snprintf(ArcCopyCmd, sizeof ArcCopyCmd, "%s %s", args[1], args[2]);
            ArcCopySet = 1;
            break;
        case CFG_ARCMOVE:
            ArcMoveExt[0] = args[0][0];
            if (argcounter == 6)
                snprintf(ArcMoveCmd, sizeof ArcMoveCmd, "%s %s %s %s", args[1], args[2], args[3], args[4]);
            else if (argcounter == 5)
                snprintf(ArcMoveCmd, sizeof ArcMoveCmd, "%s %s %s", args[1], args[2], args[3]);
            else
                snprintf(ArcMoveCmd, sizeof ArcMoveCmd, "%s %s", args[1], args[2]);
            ArcMoveSet = 1;
            break;
        case CFG_ARCOPEN:
            if (ArcOpenSet >= ARCUNPMAX)
            {
                mklog(LOG_ERROR, "%s -- too many ArcOpen lines -- %d allowed",
                        cfgline, ArcOpenSet);
                mode = -1;
                break;
            }
            ArcOpenExt[ArcOpenSet][0] = args[0][0];
            if (argcounter == 6)
                snprintf(ArcOpenCmd[ArcOpenSet], sizeof ArcOpenCmd[ArcOpenSet], "%s %s %s %s", args[1], args[2], args[3], args[4]);
            else if (argcounter == 5)
                snprintf(ArcOpenCmd[ArcOpenSet], sizeof ArcOpenCmd[ArcOpenSet], "%s %s %s", args[1], args[2], args[3]);
            else
                snprintf(ArcOpenCmd[ArcOpenSet], sizeof ArcOpenCmd[ArcOpenSet], "%s %s", args[1], args[2]);
            ArcOpenSet++;
            break;
        case CFG_BATCHFILE:
            strlcpy(BatchFile, args[0], sizeof BatchFile);
            break;
        case CFG_BAUDRATE:
            /* Parse comma separated list of baudrates, maximum twelve */
            if (argcounter != 2)
            {
                mklog(LOG_ERROR, "%s -- wrong number of arguments", cfgline);
                mode = -1;
            }
            for (i = 0; i < MAX_BAUDRATES; i++)
                nl_baudrate[i] = 0;
            i = 0;
            while (i < MAX_BAUDRATES)
            {
                if (i == 0)
                    p = strtok(args[0], ",\n\r\0");
                else
                    p = strtok(NULL, ",\n\r\0");
                if (p == NULL)
                    break;
                for (j = 0; j < (int)strlen(p); j++)
                {
                    if (! isdigit((unsigned char)p[j]))
                    {
                        mklog(LOG_ERROR, "%s -- invalid baudrate argument", cfgline);
                        mode = -1;
                        break;
                    }
                }
                if (mode == -1)
                    break;
                nl_baudrate[i] = atoi(p);
                i++;
            }
            break;
        case CFG_LOGFILE:
            strlcpy(LogFile, args[0], sizeof LogFile);
            /* Now we know the logfile, start logging immediately */
            mklog(LOG_LOGONLY, MAKENL_LONG_VERSION);
            mklog(LOG_INFO, "MakeNL started");
            break;
        case CFG_LOGLEVEL:
            if (args[0][0] >= '1' && args[0][0] <= '4' && args[0][1] == 0)
            {
                
                loglevel = args[0][0] - '0';
            }
            else
            {
                mklog(LOG_ERROR, "LOGLEVEL argument '%s' not in range 1 to 4", args[0]);
                mode = -1;
            }
            break;
        case CFG_CALLEDBATCHFILE:
            strlcpy(CalledBatchFile, args[0], sizeof CalledBatchFile);
            break;
        case CFG_CLEANUP:
            do_clean = 1;
            break;
        case CFG_COMMENTS:
            strlcpy(CommentsFile, args[0], sizeof CommentsFile);
            break;
        case CFG_COPYRIGHT:
            workptr = CopyrightFile;
            goto OutputFile;
        case CFG_EPILOG:
            workptr = EpilogFile;
            goto OutputFile;
        case CFG_NAME:
            sscanf(cfgline, "%*s %15s", namebuf);
            break;
        case CFG_MAKE:
            MakeType = xlate_switch(strupper(args[0]), MakeTypes);
            if (MakeType == -1)
            {
                mklog(LOG_ERROR, "%s -- Don't know how to make '%s'",
                        cfgline, args[0]);
                mode = -1;
            }
            if (MakeType != LEVEL_TOP) /* no composite (or master) list */
            {
                if (argcounter == 2)
                    workptr = "None";
                else
                    workptr = args[1];
                if (getnodenum(&workptr, &MakeType, &MakeNum) != 0)
                {
                    OutputErrorLine(stderr, cfgline, "", "\n", NULL);
                    mode = -1;
                }
                if (argcounter == 4) /* Source file given */
                {
                    if (filenodir(args[2]))
		    {
                        strlcpy(MakeSourceFile, args[2], sizeof MakeSourceFile);
		    }
                    else
                    {
                        mklog(LOG_ERROR, "%s -- Invalid file name -- '%s'",
                                cfgline, args[2]);
                        mode = -1;
                    }
                }
            }
            else /* making composite list */ if (argcounter > 2)
            {
                mklog(LOG_ERROR, "%s -- To many parameters", cfgline);
                mode = -1;
            }
            break;
        case CFG_MERGE:
            if (argcounter == 2)
	    {
                strlcpy(MergeFilename, args[0], sizeof MergeFilename);
	    }
            else
	    {
                strlcpy(MergeFilename, "NODELIST", sizeof MergeFilename);
	    }
            break;
        case CFG_MINPHONE:
            if (args[0][0] >= '1' && args[0][0] <= '9' && args[0][1] == 0)
                Minphone = args[0][0] - '0';
            else
            {
                mklog(LOG_ERROR, "MINPHONE argument '%s' not in range 1 to 9",
                    args[0]);
                mode = -1;
            }
            break;
        case CFG_ALPHAPHONE:
            if (args[0][0] >= '0' && args[0][0] < '2' && args[0][1] == 0)
                Alphaphone = args[0][0] - '0';
            else
            {
                mklog(LOG_ERROR, "ALPHAPHONE argument '%s' must be 0 or 1",
                        args[0]);
                mode = -1;
            }
            break;
        case CFG_ALLOWUNPUB:
            if (args[0][0] >= '0' && args[0][0] < '2' && args[0][1] == 0)
                Allowunpub = args[0][0] - '0';
            else
            {
                mklog(LOG_ERROR, "ALLOWUNPUB argument '%s' must be 0 or 1",
                        args[0]);
                mode = -1;
            }
            break;
        case CFG_FORCESUBMIT:
            if (args[0][0] >= '0' && args[0][0] < '2' && args[0][1] == 0)
                ForceSubmit = args[0][0] - '0';
            else
            {
                mklog(LOG_ERROR, "FORCESUBMIT argument '%s' must be 0 or 1",
                    args[0]);
                mode = -1;
            }
            break;
        case CFG_ALLOW8BIT:
            if (args[0][0] >= '0' && args[0][0] < '2' && args[0][1] == 0)
                Allow8Bit = args[0][0] - '0';
            else
            {
                mklog(LOG_ERROR, "ALLOW8BIT argument '%s' must be 0 or 1",
                    args[0]);
                mode = -1;
            }
            break;
        case CFG_REMOVEBOM:
            if (args[0][0] >= '0' && args[0][0] < '2' && args[0][1] == 0)
                RemoveBOM = args[0][0] - '0';
            else
            {
                mklog(LOG_ERROR, "REMOVEBOM argument '%s' must be 0 or 1",
                    args[0]);
                mode = -1;
            }
            break;
        case CFG_NETADDRESS:
            if (ParseAddress(args[0], MyAddress) != 0)
                goto BadAddress;
            break;
        case CFG_PROCESS:
            if ((process_day =
                 xlate_switch(strupper(args[0]), DOWSwitchTab)) == -1)
            {
                mklog(LOG_ERROR, "%s -- Invalid day of week '%s'",
                        cfgline, args[0]);
                mode = -1;
                break;
            }
            time(&thetime);
            mytime = localtime(&thetime);
            if (mytime->tm_wday != process_day)
                ShouldProcess &= ~USUAL_PROCESSING;
            break;
        case CFG_PUBLISH:
            NewExtWDay = xlate_switch(strupper(args[0]), DOWSwitchTab);
            if (NewExtWDay == -1)
            {
                mklog(LOG_ERROR, "%s -- Invalid day of week '%s'",
                        cfgline, args[0]);
                mode = -1;
                break;
            }
            time(&thetime);
            mytime = localtime(&thetime);
            if (NewExtWDay == 7)
                NewExtWDay = mytime->tm_wday;
            if (NewExtWDay == 8)
                NewExtWDay = mytime->tm_wday + 1;
            NewExtWDay = (NewExtWDay + 1) % 7; /* publishing day->new
                                                  filextension day */
            break;
        case CFG_POINTS:
            PointLevel = xlate_switch(strupper(args[0]), PointDisp);
            if (PointLevel == -1)
            {
                mklog(LOG_ERROR, "%s -- Invalid argument '%s'",
                        cfgline, args[0]);
                mode = -1;
                break;
            }
            break;
        case CFG_PRIVATE:
            PrivateLevel = xlate_switch(strupper(args[0]), PrivateDisp);
            if (PrivateLevel == -1)
            {
                mklog(LOG_ERROR, "%s -- Invalid argument '%s'",
                        cfgline, args[0]);
                mode = -1;
                break;
            }
            if (PrivateLevel > 0) /* Convert the keyword "pvt" into the
                                     flag "pvt" */
                Levels[LEVEL_PRIVATE] = "";
            break;
        case CFG_THRESHOLD:
            foo[0] = 0;
            if (sscanf
                (cfgline, "%s %ld %ld%1s", command, &ARCThreshold,
                 &DIFFThreshold, foo) != argcounter || foo[0] != 0)
            {
                mklog(LOG_ERROR, "%s -- Invalid threshold number(s)", cfgline);
                mode = -1;
            }
            if (argcounter == 2)
                DIFFThreshold = ARCThreshold * 5 / 3;
            else if (ARCThreshold > DIFFThreshold && DIFFThreshold != -1)
            {
                mklog(LOG_ERROR, "%s -- Archive threshold must not be greater than Diff threshold",
                        cfgline);
                mode = -1;
            }
            break;
        case CFG_BADFILES:
        case CFG_MASTER:
        case CFG_MAILFILES:
        case CFG_MESSAGES:
        case CFG_OUTPATH:
        case CFG_UPDATE:
        case CFG_UPLOADS:
            os_dirsep(args[0]);
            if (GetPath(args[0], switchno) == 0)
            {
                mklog(LOG_ERROR, "%s -- Invalid Path -- '%s'", cfgline, args[0]);
                mode = -1;
            }
            break;
        case CFG_OUTDIFF:
            if (strchr(args[0], '.') != NULL)
            {
                mklog(LOG_ERROR, "%s must contain a generic file name", command);
                mode = -1;
            }
            workptr = OutDiff;
            goto OutputFile;
        case CFG_OUTFILE:
            workptr = OutFile;
            goto OutputFile;
        case CFG_PROLOG:
            workptr = PrologFile;
          OutputFile:
            os_dirsep(args[0]);
            if (filenodir(args[0]))
	    {
                strlcpy(workptr, args[0], MYMAXFILE + MYMAXEXT);
	    }
            else
            {
                mklog(LOG_ERROR, "%s -- Invalid file name -- '%s'",
                        cfgline, args[0]);
                mode = -1;
            }
            break;
        case CFG_SUBMIT:
        case CFG_NOTIFY:
            notifybits = 0x111;
            workptr = NULL;
            if (argcounter >= 3)
            {
                flagnum = argcounter - 2;
                flagptr = args + 1;
                do
                {
                    notifybits |=
                        xlate_switch(strupper(*flagptr), MailFlags);
                    if (notifybits == -1)
                        workptr = *flagptr;
                    flagptr++;
                }
                while (--flagnum);
            }
            if (switchno == CFG_SUBMIT)
            {
                notifybits &= MF_SUBMIT;
                if (ParseAddress(args[0], SubmitAddress) != 0)
                {
                  BadAddress:
                    mklog(LOG_ERROR, "%s -- Invalid network address -- '%s'",
                            cfgline, args[0]);
                    mode = -1;
                    break;
                }
            }
            else
            {
                switch (xlate_switch(strupper(args[0]), NotifyType))
                {
                case -1:
                    mklog(LOG_ERROR, "%s -- Bad SEND parameter -- '%s'",
                            cfgline, args[0]);
                    mode = -1;
                    break;
                case NOTIFY_RECEIPT:
                    if ((MailerFlags & MF_ERRORS) == 0)
                        notifybits &= MF_RECEIPT | MF_ERRORS;
                    else
                        notifybits &= MF_RECEIPT;
                    break;
                case NOTIFY_ERRORS:
                    notifybits &= MF_ERRORS;
                    break;
                case NOTIFY_SELF:
                    notifybits = MF_SELF;
                    break;
                case NOTIFY_ALL:
                    notifybits &= MF_RECEIPT | MF_ERRORS;
                    break;
                }
            }
            MailerFlags |= notifybits;
            if (workptr)        /* Pointing to an invalid flag */
            {
                mklog(LOG_ERROR, "%s -- Bad message flag -- '%s'",
                        cfgline, workptr);
                mode = -1;
            }
            break;
        }
    }
  outofwhile:
    /*
     * If no archivers given in the configfile, set defaults now.
     */
    if (ArcCopySet == 0)
    {
        snprintf(ArcCopyExt, sizeof ArcCopyExt, "a");
        snprintf(ArcCopyCmd, sizeof ArcCopyCmd, "arc a");
    }
    if (ArcMoveSet == 0)
    {
        snprintf(ArcMoveExt, sizeof ArcMoveExt, "a");
        snprintf(ArcMoveCmd, sizeof ArcMoveCmd, "arc m");
    }
    if (ArcOpenSet == 0)
    {
        snprintf(ArcOpenExt[0], sizeof ArcOpenExt[0], "a");
        snprintf(ArcOpenCmd[0], sizeof ArcOpenCmd[0], "arc ew");
        ArcOpenSet = 1;
    }
    ArcOpenCnt = ArcOpenSet;

    if (CheckErrors(mode) == -1)
        die(255, "Error(s) in configuration file");
    return mode;
}

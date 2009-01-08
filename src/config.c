/* $Id: config.c,v 1.1.1.1 2009/01/08 20:07:46 mbroek Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "makenl.h"
#include "msg.h"
#include "config.h"
#include "fts5.h"
#include "proc.h"
#include "lsttool.h"
#include "fileutil.h"
#include "credits.h"
#include "mklog.h"
#include "version.h"

#ifdef MALLOC_DEBUG
#include "rmalloc.h"
#endif

#ifdef DMALLOC
#include "dmalloc.h"
#endif

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
            fprintf(stderr, "No MAKE specified -- nothing to make\n");
	    mklog(0, "No MAKE specified -- nothing to make");
            mode = -1;
        }
        else
        {
            if (mode == 0 && MakeSourceFile[0] == 0) /* No Source
                                                        specified */
            {
                fprintf(stderr, "No input file or data specified\n");
		mklog(0, "No input file or data specified");
                mode = -1;
            }
            else if (mode == CFG_DATA && MakeSourceFile[0] != 0)
            {
                fprintf(stderr,
                        "Master input file and DATA stream both specified\n");
		mklog(0, "Master input file and DATA stream both specified");
                mode = -1;
            }
            else if (OutFile[0] == 0)
            {
                fprintf(stderr, "No output file specified\n");
		mklog(0, "No output file specified");
                mode = -1;
            }
            else
            {
                if (MasterDir[0] == 0)
                {
                    fprintf(stderr,
                            "No directory for master files specified -- using \"%s\"\n",
                            CurDir);
		    mklog(0, "No directory for master files specified -- using \"%s\"",
			    CurDir);
                    strcpy(MasterDir, CurDir);
                }
                if (OutDir[0] == 0)
                {
                    fprintf(stderr,
                            "No directory for output file specified -- using \"%s\"\n",
                            MasterDir);
		    mklog(0, "No directory for output file specified -- using \"%s\"",
			    MasterDir);
                    strcpy(OutDir, MasterDir);
                }
                if (UploadDir[0] != 0 && !strcmp(UploadDir, MasterDir))
                {
                    fprintf(stderr,
                            "UPLoads and MASter both specify \"%s\"\n",
                            MasterDir);
		    mklog(0, "UPLoads and MASter both specify \"%s\"", MasterDir);
                    mode = -1;
                }
                else if (MailfileDir[0] != 0
                         && !strcmp(MailfileDir, MasterDir))
                {
                    fprintf(stderr,
                            "MAIlfiles and MASter both specify \"%s\"\n",
                            MasterDir);
		    mklog(0, "MAIlfiles and MASter both specify \"%s\"",
			    MasterDir);
                    mode = -1;
                }
                else if (UpdateDir[0] != 0
                         && !strcmp(UpdateDir, MasterDir))
                {
                    fprintf(stderr,
                            "UPDate and MASter both specify \"%s\"\n",
                            MasterDir);
		    mklog(0, "UPDate and MASter both specify \"%s\"",
			    MasterDir);
                    mode = -1;
                }
                else if (UpdateDir[0] != 0 && UploadDir[0] != 0
                         && !strcmp(UpdateDir, UploadDir))
                {
                    fprintf(stderr,
                            "UPLoads and UPDate both specify \"%s\"\n",
                            UpdateDir);
		    mklog(0, "UPLoads and UPDate both specify \"%s\"",
			    UpdateDir);
                    mode = -1;
                }
                else if (UpdateDir[0] != 0 && MailfileDir[0] != 0
                         && !filecmp(UpdateDir, MailfileDir))
                    /* FIXED BUG: The original read "MailfileDir != NULL" */
                {
                    fprintf(stderr,
                            "UPDate and MAIlfiles both specify \"%s\"\n",
                            UpdateDir);
		    mklog(0,  "UPDate and MAIlfiles both specify \"%s\"",
			    UpdateDir);
                    mode = -1;
                }
                else if (UploadDir[0] != 0 && MailfileDir[0] != 0
                         && !filecmp(UpdateDir, MailfileDir))
                {
                    fprintf(stderr,
                            "UPLoads and MAIlfiles both specify \"%s\"\n",
                            UploadDir);
		    mklog(0, "UPLoads and MAIlfiles both specify \"%s\"",
			    UploadDir);
                    mode = -1;
                }
            }
        }
        if (MakeType == 0 && Levels[LEVEL_TOP][0] == 0)
	{
            fprintf(stderr,
                    "WARNING -- No net NAME given for list header (use /N or NAME verb)\n");
	    mklog(0,  "WARNING -- No net NAME given for list header (use /N or NAME verb)");
	}
        if (PrivateLevel > 0 && PrivateLevel < MakeType)
        {
            fprintf(stderr,
                    "PRIvate phone numbers referred to too high a level\n");
	    mklog(0, "PRIvate phone numbers referred to too high a level");
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
                fprintf(stderr,
                        "No message directory specified for notification\n");
		mklog(0, "No message directory specified for notification");
                mode = -1;
            }
            else if (MyAddress[A_NET] == 0)
            {
                fprintf(stderr,
                        "No local \"From\" net address for notification messages.\n");
		mklog(0, "No local \"From\" net address for notification messages.");
                mode = -1;
            }
            else if (SubmitAddress[A_NET] != 0
                     && SubmitAddress[A_ZONE] == 0)
                SubmitAddress[A_ZONE] = MyAddress[A_ZONE];
            if (MailerFlags & MAIL_INTL && MyAddress[A_ZONE] == 0)
            {
                fprintf(stderr,
                        "Forced \"INTL\" kluge requested, but your zone is unknown.\n");
		mklog(0, "Forced \"INTL\" kluge requested, but your zone is unknown.");
                mode = -1;
            }
        }
	if ((ArcCopySet == 0) && (ArcMoveSet == 1))
	{
	    fprintf(stderr, "\"ArcMove\" keyword found but no \"ArcCopy\" keyword.\n");
	    mklog(0, "\"ArcMove\" keyword found but no \"ArcCopy\" keyword.");
	    mode = -1;
	}
	if ((ArcCopySet == 1) && (ArcMoveSet == 0))
	{
	    fprintf(stderr, "\"ArcCopy\" keyword found but no \"ArcMove\" keyword.\n");
	    mklog(0, "\"ArcCopy\" keyword found but no \"ArcMove\" keyword.");
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
    return xlate_switch(strupr(argument), swstrings);
}

unsigned int
xlate_switch(const char *swit, const struct switchstruct *desc)
{
    const char *swptr, *nameptr;
    int	    cnt;

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

static char *ProgramName(const char *argv0, char *def)
{
    static char *program = NULL;
    char *p;

    /* make a quick exit if we've been here before */

    if (program != NULL)
    {
        return program;
    }
    
    /* make a copy of the full filename */

    program = malloc(strlen(argv0) + 1);

    if (program == NULL)
    {
        /* malloc failed - we may as well return the default */

        return def;
    }

    strcpy(program, argv0);
    
    /* make program name point to the first character after the last \ */

    p = strrchr(program, '\\');

    if (p != NULL)
    {
        program = p + 1;
    }

    /* make program name point to the first character after the last / */

    p = strrchr(program, '/');

    if (p != NULL)
    {
        program = p + 1;
    }

#if PROGRAMNAME_STRIP_EXTENSION
    /* remove the extension from the filename */

    p = strrchr(program, '.');

    if (p != NULL)
    {
        *p = '\0'; 
    }
#endif
 
    if (*program == '\0')
    {
        /*
         *  somehow we ended up with an empty string!
         *  return the default instead
         */

        return def;
    }

#if PROGRAMNAME_LOWERCASE
    p = program;

    while (*p != '\0')
    {
        *p = tolower(*p);
        p++;
    }
#endif

    return program;
}

static void showusage(char *argv0)
{
   die(255, 1,
     "\n"
     "\n"
     "Usage: %s [<config file>] [-p|-t] [-m[=<nodelist>]] [-n=<netname>] [-d]" "\n"
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
     "                 line \"Nodelist for ...\". The -name switch may be used to" "\n"
     "                 insert a network name in front of the first word in that" "\n"
     "                 line. If the specified replacement name contains spaces," "\n"
     "                 the entire parameter must be enclosed in quotes." "\n"
     "\n"
     "  -d (-debug)    Enable debugging output for some functions." "\n"
     "\n"
     "  -c (-credits)  Who made this possible." "\n",

     ProgramName(argv0, "makenl")
   );
}

void DoCmdLine(char **argv, char **cfgfilename)
{
    char *argv0, *tmpptr, *valueptr;

    argv0 = argv[0];

    while (*(++argv) != NULL)
    {
        switch (getswitch(*argv, SwitchXLate, &valueptr))
        {
        default:
            showusage(argv0);
            break;

        case 'C':
            die(0, 1, MAKENL_CREDITS);
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
            strcpy(MergeFilename, tmpptr);
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
            MakenlDebug = 1;
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
    {"NETADRESS", 3, CFG_NETADRESS},
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
    {"ALLOWUNPUB", 4, CFG_ALLOWUNPUB},
    {"LOGFILE", 4, CFG_LOGFILE},
    {"LOGLEVEL", 4, CFG_LOGLEVEL},
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
  {2, 2},			/* LOGFile pfile */
  {2, 2},			/* LOGLevel 1..4 - default 1 */
  {2, 2}			/* FORcesubmit 1 or 0 - default 0 */
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
        strcpy(cfgSplit, cfgline);
        command = strtok(cfgSplit, cfgspacechars); /* Space and Tab */
        if (!command)
            continue;
        switchno = xlate_switch(strupr(command), CfgEntries);
        if (switchno == -1)
        {
            fprintf(stderr, "%s\n  -- Unknown keyword -- \"%s\"\n",
                    cfgline, command);
	    mklog(0, "%s  -- Unknown keyword -- \"%s\"",
		    cfgline, command);
            mode = -1;
            break;
        }
        argcounter = 1;
        while (argcounter < arglimit[switchno].max)
        {
            if (!(args[argcounter - 1] = strtok(NULL, cfgspacechars)))
                break;
            argcounter++;
        }
        if (arglimit[switchno].min > argcounter)
        {
            fprintf(stderr, "%s\n -- Too few parameters\n", cfgline);
	    mklog(0, "%s -- Too few parameters\n", cfgline);
            mode = -1;
            continue;
        }
        if (argcounter == arglimit[switchno].max && strtok(NULL, cfgspacechars)) /* Still 
                                                                                    more 
                                                                                    arguments... 
                                                                                  */
        {
            fprintf(stderr, "%s\n -- Too many parameters\n", cfgline);
	    mklog(0, "%s -- Too many parameters", cfgline);
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
		sprintf(ArcCopyCmd, "%s %s %s %s", args[1], args[2], args[3], args[4]);
	    else if (argcounter == 5)
		sprintf(ArcCopyCmd, "%s %s %s", args[1], args[2], args[3]);
            else
		sprintf(ArcCopyCmd, "%s %s", args[1], args[2]);
	    ArcCopySet = 1;
            break;
        case CFG_ARCMOVE:
            ArcMoveExt[0] = args[0][0];
	    if (argcounter == 6)
		sprintf(ArcMoveCmd, "%s %s %s %s", args[1], args[2], args[3], args[4]);
	    else if (argcounter == 5)
		sprintf(ArcMoveCmd, "%s %s %s", args[1], args[2], args[3]);
            else
		sprintf(ArcMoveCmd, "%s %s", args[1], args[2]);
	    ArcMoveSet = 1;
            break;
        case CFG_ARCOPEN:
	    if (ArcOpenSet >= ARCUNPMAX)
	    {
		fprintf(stderr, "%s\n -- too many ArcOpen lines -- %d allowed\n",
		                            cfgline, ArcOpenSet);
		mklog(0, "%s -- too many ArcOpen lines -- %d allowed",
			cfgline, ArcOpenSet);
	        mode = -1;
		break;
	    }
            ArcOpenExt[ArcOpenSet][0] = args[0][0];
	    if (argcounter == 6)
		sprintf(ArcOpenCmd[ArcOpenSet], "%s %s %s %s", args[1], args[2], args[3], args[4]);
	    else if (argcounter == 5)
		sprintf(ArcOpenCmd[ArcOpenSet], "%s %s %s", args[1], args[2], args[3]);
            else
		sprintf(ArcOpenCmd[ArcOpenSet], "%s %s", args[1], args[2]);
	    ArcOpenSet++;
            break;
        case CFG_BATCHFILE:
            strcpy(BatchFile, args[0]);
            break;
	case CFG_BAUDRATE:
	    /* Parse comma separated list of baudrates, maximum twelve */
	    if (argcounter != 2)
	    {
		fprintf(stderr, "%s -- wrong number of arguments\n", cfgline);
		mklog(0, "%s -- wrong number of arguments", cfgline);
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
			fprintf(stderr, "%s -- invalid baudrate argument\n", cfgline);
			mklog(0, "%s -- invalid baudrate argument", cfgline);
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
	    strcpy(LogFile, args[0]);
	    /*
	     * Now we know the logfile, start logging immediatly.
	     */
	    mklog(1, "");
	    mklog(1, "MakeNL %s (%s %s) start", MAKENL_VERSION, MAKENL_OS, MAKENL_CC);
	    break;
	case CFG_LOGLEVEL:
            if (args[0][0] >= '1' && args[0][0] <= '4' && args[0][1] == 0)
		loglevel = args[0][0] - '0';
	    else
	    {
		fprintf(stderr,
		    "LOGLEVEL argument \"%s\" not in range 1 to 4\n", args[0]);
		mklog(0, "LOGLEVEL argument \"%s\" not in range 1 to 4", args[0]);
		mode = -1;
	    }
	    break;
        case CFG_CALLEDBATCHFILE:
            strcpy(CalledBatchFile, args[0]);
            break;
        case CFG_CLEANUP:
            do_clean = 1;
            break;
        case CFG_COMMENTS:
            strcpy(CommentsFile, args[0]);
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
            MakeType = xlate_switch(strupr(args[0]), MakeTypes);
            if (MakeType == -1)
            {
                fprintf(stderr, "%s\n -- Don't know how to make \"%s\"\n",
                        cfgline, args[0]);
		mklog(0, "%s -- Don't know how to make \"%s\"",
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
                    OutputErrorLine(stderr, cfgline, "\n", "\n", NULL);
                    mode = -1;
                }
                if (argcounter == 4) /* Source file given */
                {
                    if (filenodir(args[2]))
                        strcpy(MakeSourceFile, args[2]);
                    else
                    {
                        fprintf(stderr,
                                "%s\n -- Invalid file name -- \"%s\"\n",
                                cfgline, args[2]);
			mklog(0, "%s -- Invalid file name -- \"%s\"",
				cfgline, args[2]);
                        mode = -1;
                    }
                }
            }
            else /* making composite list */ if (argcounter > 2)
            {
                fprintf(stderr, "%s\n -- To many parameters\n", cfgline);
		mklog(0, "%s -- To many parameters", cfgline);
                mode = -1;
            }
            break;
        case CFG_MERGE:
            if (argcounter == 2)
                strcpy(MergeFilename, args[0]);
            else
                strcpy(MergeFilename, "NODELIST");
            break;
        case CFG_MINPHONE:
            if (args[0][0] >= '1' && args[0][0] <= '9' && args[0][1] == 0)
                Minphone = args[0][0] - '0';
            else
            {
                fprintf(stderr,
                        "MINPHONE argument \"%s\" not in range 1 to 9\n",
                        args[0]);
		mklog(0, "MINPHONE argument \"%s\" not in range 1 to 9",
		    args[0]);
                mode = -1;
            }
            break;
        case CFG_ALPHAPHONE:
            if (args[0][0] >= '0' && args[0][0] < '2' && args[0][1] == 0)
                Alphaphone = args[0][0] - '0';
            else
            {
                fprintf(stderr,
                        "ALPHAPHONE argument \"%s\" must be 0 or 1\n",
                        args[0]);
		mklog(0, "ALPHAPHONE argument \"%s\" must be 0 or 1",
			args[0]);
                mode = -1;
            }
            break;
        case CFG_ALLOWUNPUB:
            if (args[0][0] >= '0' && args[0][0] < '2' && args[0][1] == 0)
                Allowunpub = args[0][0] - '0';
            else
            {
                fprintf(stderr,
                        "ALLOWUNPUB argument \"%s\" must be 0 or 1\n",
                        args[0]);
		mklog(0, "ALLOWUNPUB argument \"%s\" must be 0 or 1",
			args[0]);
                mode = -1;
            }
            break;
	case CFG_FORCESUBMIT:
	    if (args[0][0] >= '0' && args[0][0] < '2' && args[0][1] == 0)
		ForceSubmit = args[0][0] - '0';
	    else
	    {
		fprintf(stderr,
		    "FORCESUBMIT argument \"%s\" must be 0 or 1\n",
		    args[0]);
		mklog(0, "FORCESUBMIT argument \"%s\" must be 0 or 1",
		    args[0]);
		mode = -1;
	    }
	    break;
        case CFG_NETADRESS:
            if (ParseAddress(args[0], MyAddress) != 0)
                goto BadAddress;
            break;
        case CFG_PROCESS:
            if ((process_day =
                 xlate_switch(strupr(args[0]), DOWSwitchTab)) == -1)
            {
                fprintf(stderr, "%s\n -- Invalid day of week \"%s\"\n",
                        cfgline, args[0]);
		mklog(0, "%s -- Invalid day of week \"%s\"",
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
            NewExtWDay = xlate_switch(strupr(args[0]), DOWSwitchTab);
            if (NewExtWDay == -1)
            {
                fprintf(stderr, "%s\n -- Invalid day of week \"%s\"\n",
                        cfgline, args[0]);
		mklog(0, "%s -- Invalid day of week \"%s\"",
			cfgline, args[0]);
                mode = -1;
                break;
            }
            NewExtWDay = (NewExtWDay + 1) % 7; /* publishing day->new
                                                  filextension day */
            break;
        case CFG_POINTS:
            PointLevel = xlate_switch(strupr(args[0]), PointDisp);
            if (PointLevel == -1)
            {
                fprintf(stderr, "%s\n -- Invalid argument \"%s\"\n",
                        cfgline, args[0]);
		mklog(0, "%s -- Invalid argument \"%s\"",
			cfgline, args[0]);
                mode = -1;
                break;
            }
            break;
        case CFG_PRIVATE:
            PrivateLevel = xlate_switch(strupr(args[0]), PrivateDisp);
            if (PrivateLevel == -1)
            {
                fprintf(stderr, "%s\n -- Invalid argument \"%s\"\n",
                        cfgline, args[0]);
		mklog(0, "%s -- Invalid argument \"%s\"",
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
                fprintf(stderr, "%s\n -- Invalid threshold number(s)\n",
                        cfgline);
		mklog(0, "%s -- Invalid threshold number(s)", cfgline);
                mode = -1;
            }
            if (argcounter == 2)
                DIFFThreshold = ARCThreshold * 5 / 3;
            else if (ARCThreshold > DIFFThreshold && DIFFThreshold != -1)
            {
                fprintf(stderr,
                        "%s\n -- Archive threshold must not be greater than Diff threshold\n",
                        cfgline);
		mklog(0, "%s -- Archive threshold must not be greater than Diff threshold",
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
            os_filecanonify(args[0]);
            if (GetPath(args[0], switchno) == 0)
            {
                fprintf(stderr, "%s\n -- Invalid Path -- \"%s\"\n",
                        cfgline, args[0]);
		mklog(0, "%s -- Invalid Path -- \"%s\"",
			cfgline, args[0]);
                mode = -1;
            }
            break;
        case CFG_OUTDIFF:
            if (strchr(args[0], '.') != NULL)
            {
                fprintf(stderr, "%s must contain a generic file name\n",
                        command);
		mklog(0, "%s must contain a generic file name", command);
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
            os_filecanonify(args[0]);
            if (filenodir(args[0]))
                strcpy(workptr, args[0]);
            else
            {
                fprintf(stderr, "%s\n -- Invalid file name -- \"%s\"\n",
                        cfgline, args[0]);
		mklog(0, "%s -- Invalid file name -- \"%s\"",
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
                        xlate_switch(strupr(*flagptr), MailFlags);
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
                    fprintf(stderr,
                            "%s\n -- Invalid network address -- \"%s\"\n",
                            cfgline, args[0]);
		    mklog(0, "%s -- Invalid network address -- \"%s\"",
			    cfgline, args[0]);
                    mode = -1;
                    break;
                }
            }
            else
            {
                switch (xlate_switch(strupr(args[0]), NotifyType))
                {
                case -1:
                    fprintf(stderr,
                            "%s\n -- Bad SEND parameter -- \"%s\"\n",
                            cfgline, args[0]);
		    mklog(0, "%s -- Bad SEND parameter -- \"%s\"",
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
                fprintf(stderr, "%s\n -- Bad message flag -- \"%s\"\n",
                        cfgline, workptr);
		mklog(0, "%s -- Bad message flag -- \"%s\"",
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
	sprintf(ArcCopyExt, "a");
	sprintf(ArcCopyCmd, "arc a");
    }
    if (ArcMoveSet == 0)
    {
	sprintf(ArcMoveExt, "a");
	sprintf(ArcMoveCmd, "arc m");
    }
    if (ArcOpenSet == 0)
    {
	sprintf(ArcOpenExt[0], "a");
	sprintf(ArcOpenCmd[0], "arc ew");
	ArcOpenSet = 1;
    }
    ArcOpenCnt = ArcOpenSet;

    if (CheckErrors(mode) == -1)
        die(0xFF, 1, "Errors in configuration file");
    return mode;
}

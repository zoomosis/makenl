/* $Id: msgtool.c,v 1.6 2004/07/15 17:44:02 ozzmosis Exp $ */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "makenl.h"
#include "fts5.h"
#include "msg.h"
#include "fileutil.h"

#ifdef MALLOC_DEBUG
#include "rmalloc.h"
#endif

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#define MSG_CRASH    0x0002
#define MSG_PRIVATE  0x0001
#define MSG_FILE     0x0010
#define MSG_KILLSENT 0x0080
#define MSG_LOCAL    0x0100
#define MSG_HOLD     0x0400

struct MsgHdr
{
    char From[36], To[36], Subject[72], Date[20];
    short TimesRead, DestNode, OrigNode, Cost, OrigNet, DestNet;
    short DestZone, OrigZone, DestPoint, OrigPoint, Reply;
    short Attribute, NextReply;
};

int UsualMSGFlags;
int MailerFlags;
int NotifyAddress[3];
int SubmitAddress[3];
int MyAddress[3];

static char *MakeMSGFilename(char *outbuf, int num);
static struct MsgHdr MyHeader =
    { "MakeNL", "Coordinator", "subj", "date", 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0,
    0, 0
};

static int MSGnum;
static FILE *MailFILE;
static int MSGFlags;

static int SearchMaxMSG(const char *path)
{
    char *filename;
    int maxnum = 0;
    int aktnum;
    struct _filefind f;
    char searchmask[MYMAXDIR];

    myfnmerge(searchmask, NULL, NULL, "*", "MSG");
    for (filename = os_findfirst(&f, path, searchmask);
         filename != NULL; filename = os_findnext(&f))
    {
        getnumber(filename, &aktnum);
        if (aktnum > maxnum)
            maxnum = aktnum;
    }
    os_findclose(&f);

    return maxnum;
}

int ParseAddress(const char *string, int out[3])
{
    char foo[2];
    int temp[3];

    foo[0] = 0;
    if (sscanf
        (string, "%d:%d/%d%1s", &temp[A_ZONE], &temp[A_NET], &temp[A_NODE],
         foo) == 3)
    {
        if (foo[0] != 0 || temp[A_ZONE] == 0 || temp[A_NET] == 0)
            return -1;
    }
    else if (sscanf(string, "%d/%d%1s", &temp[A_NET], &temp[A_NODE], foo)
             == 2)
    {
        temp[A_ZONE] = 0;
        if (foo[0] != 0)
            return -1;
        if (temp[A_NET] == 0)
            return -1;
    }
    else if (sscanf(string, "%d%1s", &temp[A_NODE], foo) != 0)
    {
        if (foo[0] != 0)
            return -1;
        temp[A_NET] = 0;
        temp[A_ZONE] = 0;
    }
    else
        return -1;
    memcpy(out, temp, 3 * sizeof(int));

    return 0;
}

FILE *OpenMSGFile(int adress[3], char *filename)
{
    char intlline[46];
    char filenamebuf[MYMAXDIR];
    int intl;

    if (filename)
    {
        strcpy(MyHeader.Subject, filename);
        MSGFlags = (MailerFlags & MF_SUBMIT) >> MF_SHIFT_SUBMIT;
        MyHeader.Attribute = (MSGFlags & MF_CRASH ? MSG_CRASH : 0) |
            (MSGFlags & MF_HOLD ? MSG_HOLD : 0) |
            MSG_PRIVATE | MSG_FILE | MSG_KILLSENT | MSG_LOCAL;
    }
    else
    {
        MyHeader.Attribute = MSG_PRIVATE | MSG_KILLSENT | MSG_LOCAL;
        sprintf(MyHeader.Subject, "%s received", WorkFile);
        MSGFlags = UsualMSGFlags;
    }
    if (!MSGFlags)
        return (MailFILE = NULL);
    if (!adress[A_ZONE])
        adress[A_ZONE] = MyAddress[A_ZONE];
    if (MyAddress[A_ZONE] == adress[A_ZONE])
    {
        intlline[0] = 0;
        MyHeader.DestNet = adress[A_NET];
        MyHeader.DestNode = adress[A_NODE];
        intl = MailerFlags & (MF_INTL |
                              (MF_INTL << MF_SHIFT_ERRORS) |
                              (MF_INTL << MF_SHIFT_SUBMIT));
    }
    else
    {
        MyHeader.DestNet = MyAddress[A_ZONE];
        MyHeader.DestNode = adress[A_ZONE];
        intl = 1;
    }
    if (intl)
    {
        if (MyAddress[A_ZONE] == 0)
        {
            fprintf(stdout,
                    "\nWARNING -- Don't know your zone, can't send interzone message to %d:%d/%d\n\n",
                    adress[A_ZONE], adress[A_NET], adress[A_NODE]);
            return (MailFILE = NULL);
        }
        sprintf(intlline, "\x01INTL %d:%d/%d %d:%d/%d\r\n", adress[A_ZONE],
                adress[A_NET], adress[A_NODE], MyAddress[A_ZONE],
                MyAddress[A_NET], MyAddress[A_NODE]);
    }
    MailFILE = fopen(MakeMSGFilename(filenamebuf, ++MSGnum), "wb");
    if (!MailFILE)
        die(254, 1, "Cannot create %s", filenamebuf);
    fwrite(&MyHeader, sizeof(MyHeader), 1, MailFILE);
    fputs(intlline, MailFILE);
    if (!filename)
        return MailFILE;
    fclose(MailFILE);
    return (FILE *) ! NULL;     /* Just say OK - but it *smells* */
}

FILE *CloseMSGFile(int status)
{
    char filename[MYMAXDIR];

    if (MailFILE != NULL)
    {
        if (status >= 0)
        {
            if (status != 0)
            {
                MSGFlags >>= MF_SHIFT_ERRORS; /* Use the error flags */
                strcat(MyHeader.Subject, " with errors");
            }
            else
                MSGFlags &= MF_RECEIPT;
            if (MSGFlags)
            {
                putc(0, MailFILE);
                fseek(MailFILE, 0L, SEEK_SET);
                MyHeader.Attribute |=
                    (MSGFlags & MF_CRASH ? MSG_CRASH : 0) | (MSGFlags &
                                                             MF_HOLD ?
                                                             MSG_HOLD : 0);
                fwrite(&MyHeader, sizeof(MyHeader), 1, MailFILE);
            }
            else
                status = -1;
        }
        fclose(MailFILE);
        if (status < 0)
            unlink(MakeMSGFilename(filename, MSGnum--));
    }
    MailFILE = NULL;
    return MailFILE;
}

void BuildHeaders(void)
{
    static char *DOWNames[] =
        { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    static char *MonthNames[12] =
        { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
        "Sep", "Oct", "Nov", "Dec"
    };
    time_t akt_time;
    struct tm *the_time;

    time(&akt_time);
    the_time = localtime(&akt_time);
    /* BUG FIXED: Y2K-bug changed tm_year to tm_year % 100 and its format
       to %02d */
    sprintf(MyHeader.Date, "%s %2d %s %02d %02d:%02d",
            DOWNames[the_time->tm_wday], the_time->tm_mday,
            MonthNames[the_time->tm_mon], the_time->tm_year % 100,
            the_time->tm_hour, the_time->tm_min);
    MyHeader.OrigNet = MyAddress[A_NET];
    MyHeader.OrigNode = MyAddress[A_NODE];
    MSGnum = SearchMaxMSG(MessageDir);
}

char *MakeMSGFilename(char *outbuf, int num)
{
    char buffer[6];

    sprintf(buffer, "%u", num);
    myfnmerge(outbuf, NULL, MessageDir, buffer,
#ifndef __unix__
              "MSG");
#else
              "msg");
#endif
    return outbuf;
}

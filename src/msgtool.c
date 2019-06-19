#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include "makenl.h"
#include "fts5.h"
#include "msg.h"
#include "fileutil.h"
#include "mklog.h"
#include "version.h"
#include "strtool.h"
#include "snprintf.h"

#define MSG_CRASH    0x0002
#define MSG_PRIVATE  0x0001
#define MSG_FILE     0x0010
#define MSG_KILLSENT 0x0080
#define MSG_LOCAL    0x0100
#define MSG_HOLD     0x0200

int UsualMSGFlags;
int MailerFlags;
int NotifyAddress[3];
int SubmitAddress[3];
int MyAddress[3];

static int MSGnum;
static FILE *MailFILE;
static int MSGFlags;
static unsigned char msgbuf[0xbe];

/* Get unique sequence number for MSGID */

/*
 *  TODO: Write sequence.dat as a text file instead of binary,
 *  to avoid data width and endianness problems.
 *
 *  TODO: Also stop using time() as it's not necessarily 32-bit.
 *  - ozzmosis 2018-11-25
 */

static unsigned long NewMSGID(void)
{
    unsigned long seq;
    char filename[MYMAXPATH];
    FILE *fp;
    int rc;

    snprintf(filename, sizeof filename, "%s" DIRSEP "sequence.dat", MasterDir);

    mklog(LOG_DEBUG, "MSGID sequence filename is '%s'", filename);

    fp = fopen(filename, "r+");

    if (fp == NULL)
    {
        seq = (unsigned long) time(NULL);

        mklog(LOG_DEBUG, "Newly-created MSGID sequence is %08lx", seq);

        fp = fopen(filename, "w+");

        if (fp == NULL)
        {
            mklog(LOG_ERROR, "$Can't create '%s' for writing", filename);
            return seq;
        }

        rc = fwrite(&seq, 1, sizeof seq, fp);

        mklog(LOG_DEBUG, "fwrite(&seq, 1, sizeof seq, fp) returned %d", rc);

        rc = fclose(fp);

        if (rc != 0)
        {
            mklog(LOG_DEBUG, "$fclose() failed for '%s'", filename);
        }

        return seq;
    }

    rc = fread(&seq, 1, sizeof seq, fp);

    seq++;

    mklog(LOG_DEBUG, "Incremented MSGID sequence is %08lx", seq);

    rc = fseek(fp, 0L, SEEK_SET);

    if (rc != 0)
    {
        mklog(LOG_DEBUG, "$fseek failed for '%s'", filename);
    }

    rc = fwrite(&seq, 1, sizeof seq, fp);
    mklog(LOG_DEBUG, "fwrite(&seq, 1, sizeof seq, fp) returned %d", rc);

    rc = fclose(fp);

    if (rc != 0)
    {
        mklog(LOG_DEBUG, "$fclose() failed for '%s'", filename);
    }

    return seq;
}

static int SearchMaxMSG(const char *path)
{
    char *filename;
    int maxnum;
    int aktnum;
    struct _filefind f;
    char searchmask[MYMAXDIR];

    maxnum = 0;
    myfnmerge(searchmask, NULL, NULL, "*", "msg");
    filename = os_findfirst(&f, path, searchmask);

    while (filename != NULL)
    {
        getnumber(filename, &aktnum);
        if (aktnum > maxnum)
        {
            maxnum = aktnum;
        }
        filename = os_findnext(&f);
    }
    os_findclose(&f);

    mklog(LOG_DEBUG, "SearchMaxMSG: path='%s', result=%d", make_str_safe(path), maxnum);

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

static char *MakeMSGFilename(char *outbuf, int num)
{
    char buffer[MYMAXDIR];

    snprintf(buffer, sizeof buffer, "%u", num);
    myfnmerge(outbuf, NULL, MessageDir, buffer, "msg");
    mklog(LOG_DEBUG, "MakeMSGFilename: num=%d MSGnum=%d", num, MSGnum);
    return outbuf;
}

FILE *OpenMSGFile(int address[3], char *filename)
{
    /* static char *DOWNames[] =
            { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" }; */
    /* Unused - now using FTS-1 date/time field vs. SEAdog format */
    static char *MonthNames[12] =
            { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    time_t akt_time;
    struct tm *the_time;
    char intlline[46];
    char filenamebuf[MYMAXDIR], subject[72], date[21];
    int intl, temp;

    if (!address[A_ZONE]) /* If dest. zone is 0, assume it's the same as ours. */
        address[A_ZONE] = MyAddress[A_ZONE];

    mklog(LOG_DEBUG, "OpenMSGFile entered");
    mklog(LOG_DEBUG, "OpenMSGFile: %d:%d/%d filename='%s'", address[A_ZONE],
            address[A_NET], address[A_NODE], make_str_safe(filename));

    mklog(LOG_DEBUG, "SearchMaxMSG('%s')", MessageDir);
    MSGnum = SearchMaxMSG(MessageDir);
    mklog(LOG_DEBUG, "OpenMSGFile: MSGnum is set to %d", MSGnum);

    memset(&filenamebuf, 0, sizeof(filenamebuf));
    memset(&msgbuf, 0, sizeof(msgbuf));

    memcpy(&msgbuf[0x00], (char *)"MakeNL ", 7);
    memcpy(&msgbuf[0x07], MAKENL_VERSION, sizeof(MAKENL_VERSION));
    memcpy(&msgbuf[0x24], (char *)"Coordinator", 11);

    if (filename)
    {
        snprintf(subject, sizeof subject, "%s", filename);
        MSGFlags = (MailerFlags & MF_SUBMIT) >> MF_SHIFT_SUBMIT;
        temp = (MSGFlags & MF_CRASH ? MSG_CRASH : 0) |
            (MSGFlags & MF_HOLD ? MSG_HOLD : 0) |
            MSG_PRIVATE | MSG_FILE | MSG_KILLSENT | MSG_LOCAL;
    }
    else
    {
        temp = MSG_PRIVATE | MSG_KILLSENT | MSG_LOCAL;
        snprintf(subject, sizeof subject, "%s received", WorkFile);
        MSGFlags = UsualMSGFlags;
    }
    memcpy(&msgbuf[0x48], subject, strlen(subject));

    time(&akt_time);
    the_time = localtime(&akt_time);
    /* BUG FIXED: Y2K-bug changed tm_year to tm_year % 100 and its format
     *        to %02d */
    snprintf(date, sizeof date, "%02d %s %02d  %02d:%02d:%02d",
            the_time->tm_mday, MonthNames[the_time->tm_mon],
            the_time->tm_year % 100, the_time->tm_hour, the_time->tm_min,
            the_time->tm_sec); /* Switch to FTS-1 date/time vs. SEAdog */
    memcpy(&msgbuf[0x90], date, 20);
    msgbuf[0xba] = (unsigned char)(temp & 0x00ff);     /* Attribute */
    msgbuf[0xbb] = (unsigned char)((temp & 0xff00) >> 8);

    if (!MSGFlags)
        return (MailFILE = NULL);
    intlline[0] = 0; /* Initialize INTL line. */
    /* Build message destination address. */
    msgbuf[0xae] = (unsigned char)(address[A_NET] & 0x00ff);        /* destNet        */
    msgbuf[0xaf] = (unsigned char)((address[A_NET] & 0xff00) >> 8);
    msgbuf[0xa6] = (unsigned char)(address[A_NODE] & 0x00ff);        /* destNode        */
    msgbuf[0xa7] = (unsigned char)((address[A_NODE] & 0xff00) >> 8);
    msgbuf[0xb0] = (unsigned char)(address[A_ZONE] & 0x00ff);        /* destZone        */
    msgbuf[0xb1] = (unsigned char)((address[A_ZONE] & 0xff00) >> 8);
    if (MyAddress[A_ZONE] == address[A_ZONE]) /* Intra-zone message. */
    {
        intl = MailerFlags & (MF_INTL |
                              (MF_INTL << MF_SHIFT_ERRORS) |
                              (MF_INTL << MF_SHIFT_SUBMIT));
    }
    else
    {
        intl = 1;
    }
    /* Build message origin address. */
    msgbuf[0xa8] = (unsigned char)(MyAddress[A_NODE] & 0x00ff);        /* origNode        */
    msgbuf[0xa9] = (unsigned char)((MyAddress[A_NODE] & 0xff00) >> 8);
    msgbuf[0xac] = (unsigned char)(MyAddress[A_NET] & 0x00ff);                /* origNet        */
    msgbuf[0xad] = (unsigned char)((MyAddress[A_NET] & 0xff00) >> 8);
    msgbuf[0xb2] = (unsigned char)(MyAddress[A_ZONE] & 0x00ff);        /* origZone        */
    msgbuf[0xb3] = (unsigned char)((MyAddress[A_ZONE] & 0xff00) >> 8);

    if (intl)
    {
        if (MyAddress[A_ZONE] == 0)
        {
            mklog(LOG_INFO, "WARNING -- Don't know your zone, can't send interzone message to %d:%d/%d",
              address[A_ZONE], address[A_NET], address[A_NODE]);
            return (MailFILE = NULL);
        }
        snprintf(intlline, sizeof intlline, "\x01INTL %d:%d/%d %d:%d/%d\r\n", address[A_ZONE],
                address[A_NET], address[A_NODE], MyAddress[A_ZONE],
                MyAddress[A_NET], MyAddress[A_NODE]);
    }
    MailFILE = fopen(MakeMSGFilename(filenamebuf, MSGnum + 1), "wb");
    if (MailFILE == NULL)
        die(254, "$Cannot create file '%s'", filenamebuf);
    MSGnum++;
    mklog(LOG_DEBUG, "OpenMSGFile: opened '%s', MSGnum %d", filenamebuf, MSGnum);

    fwrite(&msgbuf, sizeof(msgbuf), 1, MailFILE);
    fputs(intlline, MailFILE);
    fprintf(MailFILE, "\x01MSGID: %d:%d/%d %08lx\r\n", MyAddress[A_ZONE],
            MyAddress[A_NET], MyAddress[A_NODE], NewMSGID());
    if (!filename)
    {
        mklog(LOG_DEBUG, "OpenMSGFile: return with open MailFILE");
        return MailFILE;
    }
    fclose(MailFILE);
    mklog(LOG_DEBUG, "OpenMSGFile: closed, seems Ok");

    /*
     *  Fell through. The file is now closed but we still want to return
     *  "success" using a non-NULL pointer, so we'll use stdout. This
     *  replaces an awful hack with a slightly less-awful hack.
     *
     *  At least now, if the calling function tries to write to the file
     *  pointed to by the returned value it should just send output to the
     *  screen, instead of segfaulting.
     *
     *  The program may still segfault if the code tries to read input
     *  from stdout. I'd need to check the C Standard about that...
     *
     *  - ozzmosis 2013-09-04
     */

    return stdout;
}



FILE *CloseMSGFile(int status)
{
    char filename[MYMAXDIR];
    int i, temp;

    mklog(LOG_DEBUG, "CloseMSGFile: status=%d", status);

    if (MailFILE != NULL)
    {
        mklog(LOG_DEBUG, "CloseMSGFile: MailFILE is open");
        if (status >= 0)
        {
            if (status != 0)
            {
                MSGFlags >>= MF_SHIFT_ERRORS;        /* Use the error flags            */
                for (i = 0x48; i; i++)                /* Search end of subject    */
                {
                    if (msgbuf[i] == '\0')
                    {
                        break;
                    }
                }
                memcpy(&msgbuf[i], " with errors\0", 13);
            }
            else
            {
                MSGFlags &= MF_RECEIPT;
            }
            if (MSGFlags)
            {
                mklog(LOG_DEBUG, "CloseMSGFile: MSGFlags != 0");
                putc(0, MailFILE);
                fseek(MailFILE, 0L, SEEK_SET);
                temp = (msgbuf[0xbb] << 8) + msgbuf[0xba]; /* Get current
                                                              .MSG flags */
                temp |=
                    (MSGFlags & MF_CRASH ? MSG_CRASH : 0) | (MSGFlags &
                                                             MF_HOLD ?
                                                             MSG_HOLD : 0);
                                                             /* Add CRASH
                                                                or HOLD
                                                                if needed */
                msgbuf[0xba] = (unsigned char)(temp & 0x00ff);     /* Attribute */
                msgbuf[0xbb] = (unsigned char)((temp & 0xff00) >> 8);
                fwrite(&msgbuf, sizeof(msgbuf), 1, MailFILE);
            }
            else
                status = -1;
        }
        mklog(LOG_DEBUG, "CloseMSGFile: closing file, status is now %d, MSGnum=%d", status, MSGnum);
        fclose(MailFILE);
        if (status < 0)
        {
            unlink(MakeMSGFilename(filename, MSGnum--));
            mklog(LOG_DEBUG, "CloseMSGFile: unlink %s", filename);
        }
    }
    mklog(LOG_DEBUG, "CloseMSGFile: MSGnum=%d", MSGnum);
    MailFILE = NULL;
    return MailFILE;
}

/* $Id: msgtool.c,v 1.17 2005-05-23 17:08:24 mbroek Exp $ */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "makenl.h"
#include "fts5.h"
#include "msg.h"
#include "fileutil.h"
#include "mklog.h"
#include "version.h"

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



int UsualMSGFlags;
int MailerFlags;
int NotifyAddress[3];
int SubmitAddress[3];
int MyAddress[3];

static int MSGnum;
static FILE *MailFILE;
static int MSGFlags;
static unsigned char msgbuf[0xbe];


static char *MakeMSGFilename(char *outbuf, int num);



/*
 * Get unique sequence number
 */
static unsigned long GetSequence(void)
{
    unsigned long   seq;
    char	    seqfile[MYMAXDIR];
    FILE	    *fp;
    sprintf(seqfile, "%s/sequence.dat", MasterDir);

    if ((fp = fopen(seqfile, "r+")) == NULL) {
	seq = (unsigned long)time(NULL);
	if ((fp = fopen(seqfile, "w+")) == NULL) {
	    fprintf(stderr, "Can't create %s\n", seqfile);
	    mklog(0, "Can't create %s", seqfile);
	    return seq;
	} else {
	    fwrite(&seq, 1, sizeof(seq), fp);
	    fclose(fp);
	    return seq;
	}
    } else {
	fread(&seq, 1, sizeof(seq), fp);
	seq++;
	fseek(fp, 0, SEEK_SET);
	fwrite(&seq, 1, sizeof(seq), fp);
	fclose(fp);
    }
    return seq;
}



// static 
int SearchMaxMSG(const char *path)
{
    char *filename;
    int maxnum = 0;
    int aktnum;
    struct _filefind f;
    char searchmask[MYMAXDIR];

    myfnmerge(searchmask, NULL, NULL, "*", "msg");
    for (filename = os_findfirst(&f, path, searchmask);
         filename != NULL; filename = os_findnext(&f))
    {
        getnumber(filename, &aktnum);
        if (aktnum > maxnum)
            maxnum = aktnum;
    }
    os_findclose(&f);

    mklog(4, "SearchMaxMSG: path=%s, result=%d", MAKE_SS(path), maxnum);
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
    static char *DOWNames[] =
	    { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    static char *MonthNames[12] =
	    { "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
	      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    time_t akt_time;
    struct tm *the_time;
    char intlline[46];
    char filenamebuf[MYMAXDIR], subject[72], date[21];
    int intl, temp;

    mklog(4, "OpenMSGFile entered");	// MB
    mklog(3, "OpenMSGFile: %d:%d/%d filename=%s", adress[A_ZONE],
	    adress[A_NET], adress[A_NODE], MAKE_SS(filename));

    mklog(4, "SearchMaxMSG(%s)", MAKE_SS(MessageDir)); // MB
    MSGnum = SearchMaxMSG(MessageDir);
    mklog(4, "OpenMSGFile: MSGnum is set to %d", MSGnum);

    memset(&filenamebuf, 0, sizeof(filenamebuf));
    memset(&msgbuf, 0, sizeof(msgbuf));

    memcpy(&msgbuf[0x00], (char *)"MakeNL ", 7);
    memcpy(&msgbuf[0x07], MAKENL_VERSION, sizeof(MAKENL_VERSION));
    memcpy(&msgbuf[0x24], (char *)"Coordinator", 11);

    if (filename)
    {
	sprintf(subject, "%s", filename);
        MSGFlags = (MailerFlags & MF_SUBMIT) >> MF_SHIFT_SUBMIT;
        temp = (MSGFlags & MF_CRASH ? MSG_CRASH : 0) |
            (MSGFlags & MF_HOLD ? MSG_HOLD : 0) |
            MSG_PRIVATE | MSG_FILE | MSG_KILLSENT | MSG_LOCAL;
    }
    else
    {
        temp = MSG_PRIVATE | MSG_KILLSENT | MSG_LOCAL;
        sprintf(subject, "%s received", WorkFile);
        MSGFlags = UsualMSGFlags;
    }
    memcpy(&msgbuf[0x48], subject, strlen(subject));

    time(&akt_time);
    the_time = localtime(&akt_time);
    /* BUG FIXED: Y2K-bug changed tm_year to tm_year % 100 and its format
     *        to %02d */
    sprintf(date, "%s %2d %s %02d %02d:%02d",
	    DOWNames[the_time->tm_wday], the_time->tm_mday,
	    MonthNames[the_time->tm_mon], the_time->tm_year % 100,
	    the_time->tm_hour, the_time->tm_min);
    memcpy(&msgbuf[0x90], date, 20);
    msgbuf[0xba] = (temp & 0x00ff);     /* Atribute */
    msgbuf[0xbb] = (temp & 0xff00) >> 8;

    if (!MSGFlags)
        return (MailFILE = NULL);
    if (!adress[A_ZONE])
        adress[A_ZONE] = MyAddress[A_ZONE];
    if (MyAddress[A_ZONE] == adress[A_ZONE])
    {
        intlline[0] = 0;
	msgbuf[0xae] = (adress[A_NET] & 0x00ff);	/* destNet	*/
	msgbuf[0xaf] = (adress[A_NET] & 0xff00) >> 8;
	msgbuf[0xa6] = (adress[A_NODE] & 0x00ff);	/* destNode	*/
	msgbuf[0xa7] = (adress[A_NODE] & 0xff00) >> 8;
	msgbuf[0xb0] = (adress[A_ZONE] & 0x00ff);	/* destZone	*/
	msgbuf[0xb1] = (adress[A_ZONE] & 0xff00) >> 8;
        intl = MailerFlags & (MF_INTL |
                              (MF_INTL << MF_SHIFT_ERRORS) |
                              (MF_INTL << MF_SHIFT_SUBMIT));
    }
    else
    {
	msgbuf[0xae] = (MyAddress[A_ZONE] & 0x00ff);	/* destNet	*/
	msgbuf[0xaf] = (MyAddress[A_ZONE] & 0xff00) >> 8;
	msgbuf[0xa6] = (adress[A_ZONE] & 0x00ff);	/* destNode	*/
	msgbuf[0xa7] = (adress[A_ZONE] & 0xff00) >> 8;
	msgbuf[0xb0] = (MyAddress[A_ZONE] & 0x00ff);	/* destZone	*/
	msgbuf[0xb1] = (MyAddress[A_ZONE] & 0xff00) >> 8;
        intl = 1;
    }

    msgbuf[0xa8] = (MyAddress[A_NODE] & 0x00ff);	/* origNode	*/
    msgbuf[0xa9] = (MyAddress[A_NODE] & 0xff00) >> 8;
    msgbuf[0xac] = (MyAddress[A_NET] & 0x00ff);		/* origNet	*/
    msgbuf[0xad] = (MyAddress[A_NET] & 0xff00) >> 8;
    msgbuf[0xb2] = (MyAddress[A_ZONE] & 0x00ff);	/* origZone	*/
    msgbuf[0xb3] = (MyAddress[A_ZONE] & 0xff00) >> 8;
    
    if (intl)
    {
        if (MyAddress[A_ZONE] == 0)
        {
            fprintf(stdout,
                    "\nWARNING -- Don't know your zone, can't send interzone message to %d:%d/%d\n\n",
                    adress[A_ZONE], adress[A_NET], adress[A_NODE]);
	    mklog(1, "WARNING -- Don't know your zone, can't send interzone message to %d:%d/%d",
		    adress[A_ZONE], adress[A_NET], adress[A_NODE]);
            return (MailFILE = NULL);
        }
        sprintf(intlline, "\x01INTL %d:%d/%d %d:%d/%d\r\n", adress[A_ZONE],
                adress[A_NET], adress[A_NODE], MyAddress[A_ZONE],
                MyAddress[A_NET], MyAddress[A_NODE]);
    }
    MailFILE = fopen(MakeMSGFilename(filenamebuf, MSGnum + 1), "wb");
    if (!MailFILE)
        die(254, 1, "Cannot create %s", filenamebuf);
    MSGnum++;
    mklog(3, "OpenMSGFile: opened %s, MSGnum %d", filenamebuf, MSGnum);
    
    fwrite(&msgbuf, sizeof(msgbuf), 1, MailFILE);
    fputs(intlline, MailFILE);
    fprintf(MailFILE, "\x01MSGID: %d:%d/%d %08lx\r\n", MyAddress[A_ZONE], 
	    MyAddress[A_NET], MyAddress[A_NODE], GetSequence());
    if (!filename)
    {
	mklog(4, "OpenMSGFile: return with open MailFILE");
        return MailFILE;
    }
    fclose(MailFILE);
    mklog(4, "OpenMSGFile: closed, seems Ok");
    return (FILE *) ! NULL;     /* Just say OK - but it *smells* */
}



FILE *CloseMSGFile(int status)
{
    char filename[MYMAXDIR];
    int	i, temp = 0;

    mklog(3, "CloseMSGFile: status=%d", status);

    if (MailFILE != NULL)
    {
	mklog(4, "CloseMSGFile: MailFILE is open");
        if (status >= 0)
        {
            if (status != 0)
            {
                MSGFlags >>= MF_SHIFT_ERRORS;	/* Use the error flags	    */
		for (i = 0x48; i; i++)		/* Search end of subject    */
		{
		    if (msgbuf[i] == '\0')
		    {
			break;
		    }
		}
                memcpy(&msgbuf[i], " with errors", 12);
            }
            else
	    {
                MSGFlags &= MF_RECEIPT;
	    }
            if (MSGFlags)
            {
		mklog(4, "CloseMSGFile: MSGFlags != 0");
                putc(0, MailFILE);
                fseek(MailFILE, 0L, SEEK_SET);
                temp |=
                    (MSGFlags & MF_CRASH ? MSG_CRASH : 0) | (MSGFlags &
                                                             MF_HOLD ?
                                                             MSG_HOLD : 0);
		msgbuf[0xba] = (temp & 0x00ff);     /* Atribute */
		msgbuf[0xbb] = (temp & 0xff00) >> 8;
                fwrite(&msgbuf, sizeof(msgbuf), 1, MailFILE);
            }
            else
                status = -1;
        }
	mklog(4, "CloseMSGFile: closing file, status is now %d, MSGnum=%d", status, MSGnum);
        fclose(MailFILE);
        if (status < 0)
	{
            unlink(MakeMSGFilename(filename, MSGnum--));
	    mklog(3, "CloseMSGFile: unlink %s", filename);
	}
    }
    mklog(4, "CloseMSGFile: MSGnum=%d", MSGnum);
    MailFILE = NULL;
    return MailFILE;
}



char *MakeMSGFilename(char *outbuf, int num)
{
    char buffer[6];

    sprintf(buffer, "%u", num);
    myfnmerge(outbuf, NULL, MessageDir, buffer, "msg");
    mklog(4, "MakeMSGFilenam: num=%d MSGnum=%d", num, MSGnum);
    return outbuf;
}


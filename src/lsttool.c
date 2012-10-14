/* $Id: lsttool.c,v 1.7 2012/10/14 14:56:24 ozzmosis Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "makenl.h"
#include "fileutil.h"
#include "crc16.h"
#include "lsttool.h"
#include "msg.h"
#include "mklog.h"
#include "proc.h"

#ifdef MALLOC_DEBUG
#include "rmalloc.h"
#endif

#ifdef DMALLOC
#include "dmalloc.h"
#endif

long ARCThreshold = 10000;
long DIFFThreshold = 16666;

char ArcCopyExt[ARCEXTMAX];
char ArcCopyCmd[ARCCMDMAX];
char ArcMoveExt[ARCEXTMAX];
char ArcMoveCmd[ARCCMDMAX];
char ArcOpenExt[ARCUNPMAX][ARCEXTMAX];
char ArcOpenCmd[ARCUNPMAX][ARCCMDMAX];
int  ArcOpenCnt = 0;

int ForceSubmit = 0;

#if 0
static unsigned int arcparas = 0x4000; /* 256 KBytes */

#endif
static unsigned short DiffCRC;
static char DiffLine[linelength];
static int EditCount;
static int AddLines;
static int dodiffline(int checktop, FILE * oldFILE, FILE * diffFILE);

int makearc(char *filename, int move)
{
    char ext[MYMAXEXT];
    char name[MYMAXFILE];
    char fullpath[MYMAXPATH];
    char cmdlinebuf[MYMAXPATH];
    char arccommand[ARCCMDMAX];

    if (filesize(filename) <= ARCThreshold || ARCThreshold == -1)
    {
        mklog(LOG_DEBUG, "Skip arc for %s", filename);
        return 0;
    }

    strcpy(fullpath, filename);
    myfnsplit(filename, NULL, NULL, name, ext);
    if (toupper(ext[0]) == 'D')          /* Autogenerated diff - can always be
                                   moved */
    {
        ext[1] = 'd';
        strcpy(arccommand, ArcMoveCmd); /* move instead of add */
        ext[0] = ArcMoveExt[0];
    }
    else
    {
        strcpy(arccommand, (move < 1) ? ArcCopyCmd : ArcMoveCmd);
        ext[0] = (move < 1) ? ArcCopyExt[0] : ArcMoveExt[0];
    }
    myfnmerge(fullpath, NULL, OutDir, name, ext);
    os_filecanonify(fullpath);
    os_filecanonify(filename);
    mklog(LOG_INFO, "Creating archive '%s' containing '%s'", fullpath, filename);
    sprintf(cmdlinebuf, "%s %s", fullpath, filename);
    if (os_spawn(arccommand, cmdlinebuf) != 0)
        die(0xFD, "Unable to create archive '%s'", fullpath);
    strcpy(filename, fullpath);
    return 1;
}

/* Returns:
   1 if list is unchanged,
   0 else
 */
int installlist(char *filename, char *extbfr)
{
    char *outext;
    char *oldstart;
    FILE *oldFILE = NULL;
    char *tmpstart;
    int weeksold = 0;
    FILE *tmpFILE = NULL;
    int unchanged = 0;
    char (*extptr)[MYMAXEXT];
    char tmpname[linelength];
    char tmpline[linelength];
    char oldline[linelength];

    for (extptr = OldExtensions; extptr < OldExtensions + 3; extptr++)
    {
        outext = getext(extbfr, OutFile) ? NULL : *extptr;
        myfnmerge(filename, NULL, OutDir, OutFile, outext);
        oldFILE = fopen(filename, "r");
        if (oldFILE != NULL || extbfr[0] != 0)
            break;
        weeksold++;
    }
    swapext(tmpname, filename, "$$$");
    if (oldFILE && OutDiff[0] == 0)
    {
        tmpFILE = fopen(tmpname, "r");
        if (!tmpFILE)
            die(254, "Unable to open new list '%s' for input\n",
                tmpname);
        if (fgets(tmpline, linelength, tmpFILE)
            && fgets(oldline, linelength, oldFILE)
            && (tmpstart = strrchr(tmpline, ' ')) != NULL
            && (oldstart = strrchr(oldline, ' ')) != NULL)
            while (!strcmp(oldstart, tmpstart))
            {
                tmpstart = fgets(tmpline, linelength, tmpFILE);
                oldstart = fgets(oldline, linelength, oldFILE);
                if (tmpstart == NULL || oldstart == NULL)
                {
                    unchanged = 1;
                    break;
                }
            }
        fclose(tmpFILE);
    }
    if (oldFILE)
        fclose(oldFILE);

    if (unchanged && (ShouldProcess & FORCED_PROCESSING) && ForceSubmit) 
    {
        mklog(LOG_INFO, "Unchanged output file will be forced submitted");
        unchanged = 0;
    }
    
    if (unchanged)
    {
        unlink(tmpname);
        if (extbfr[0] == 0)     /* generic filename given */
        {
            if (weeksold != 0)
            {
                swapext(tmpname, filename, OldExtensions[0]);
                rename(filename, tmpname);
                strcpy(filename, tmpname);
            }
            for (extptr = OldExtensions + weeksold + 1;
                 extptr < OldExtensions + 3; extptr++)
            {
                swapext(tmpname, filename, *extptr);
                unlink(tmpname);
            }
        }
        mklog(LOG_INFO, "Unchanged output file will NOT be submitted");
    }
    else
    {
        if (weeksold != 0)
            swapext(filename, tmpname, OldExtensions[0]);
        else
            unlink(filename);
        rename(tmpname, filename);
    }
    return unchanged;
}

static int ApplyDiff(FILE * oldFILE, char *diffname, char *outname)
{
    int firststatus;
    char *crcptr;               /* 0x02 */
    FILE *outFILE;              /* 0x04 */
    FILE *diffFILE;             /* 0x06 */
    int newcrc;                 /* 0x08 */

    mklog(LOG_DEBUG, "applydiff '%s' to '%s'", diffname, outname);

    diffFILE = fopen(diffname, "r");
    if (!diffFILE)
        die(254, "Unable to open %s for input", diffname);
    outFILE = fopen(outname, "wb");
    if (!outFILE)
        die(254, "Unable to create %s", outname);
    firststatus = dodiffline(1, oldFILE, diffFILE);
    if (firststatus == 0)       /* diff fits */
    {
        /* DiffLine now contains the new header including CRC */
        cutspaces(DiffLine);
        strcat(DiffLine, "\r\n");
        crcptr = DiffLine + strlen(DiffLine);
        while (*(--crcptr) != ' ');
        getnumber(crcptr + 1, &newcrc);
        fputs(DiffLine, outFILE);
        DiffCRC = 0;
        while (dodiffline(0, oldFILE, diffFILE) == 0)
        {
            cutspaces(DiffLine);
            strcat(DiffLine, "\r\n");
            fputs(DiffLine, outFILE);
            DiffCRC = CRC16String(DiffLine, DiffCRC);
        }
        putc('\x1A', outFILE);
    }
    fclose(outFILE);
    fclose(oldFILE);
    fclose(diffFILE);
    if (firststatus == -1
        || CRC16DoByte((char)(newcrc & 0xFF),
                       CRC16DoByte((char)(newcrc >> 8), DiffCRC)) != 0)
    {
        unlink(outname);
        return -1;
    }
    return 0;
}

static int dodiffline(int checktop, FILE * oldFILE, FILE * diffFILE)
{
    char topline[linelength];

    if (checktop)
    {
        EditCount = 0;
        if (fgets(topline, linelength, diffFILE) == NULL)
            return -1;
    }
    while (EditCount == 0)
    {
        if (fgets(DiffLine, linelength, diffFILE) == NULL)
            /* return 1 if call fails - -1 if it works */
            return 2 * !fgets(DiffLine, linelength, diffFILE) - 1;
        getnumber(DiffLine + 1, &EditCount);
        switch (DiffLine[0])
        {
        default:
            return -1;
        case 'A':
            AddLines = 1;
            break;
        case 'C':
            AddLines = 0;
            break;
        case 'D':
            if (EditCount)
                do
                {
                    if (fgets(DiffLine, linelength, oldFILE) == NULL)
                        return -1;
                    if (checktop)
                    {
                        if (strcmp(DiffLine, topline))
                            return -1;
                        checktop--;
                    }
                }
                while (--EditCount);
            break;
        }
    }
    if (fgets(DiffLine, linelength, (AddLines ? diffFILE : oldFILE)) ==
        NULL)
        return -1;
    EditCount--;
    return 0;
}



/*
 * Test unpacker, see also makenl.h for ARCUNPMAX
 */
char *unpacker(char *fn)
{
    FILE            *fp;
    unsigned char   buf[8];

    if ((fp = fopen(fn,"r")) == NULL) {
        fprintf(stderr, "Could not open file %s\n", fn);
        return NULL;
    }

    if (fread(buf,1,sizeof(buf),fp) != sizeof(buf)) {
        fprintf(stderr, "Could not read head of the file %s\n", fn);
        fclose(fp);
        return NULL;
    }
    fclose(fp);

    if (memcmp(buf,"PK\003\004",4) == 0)    return (char *)"Z";            /* ZIP        */
    if (*buf == 0x1a)                       return (char *)"A";            /* ARC        */
    if (memcmp(buf+2,"-l",2) == 0)          return (char *)"L";            /* LHA        */
    if (memcmp(buf,"ZOO",3) == 0)           return (char *)"O";            /* ZOO        */
    if (memcmp(buf,"`\352",2) == 0)         return (char *)"J";            /* ARJ        */
    if (memcmp(buf,"Rar!",4) == 0)          return (char *)"R";            /* RAR        */
    if (memcmp(buf,"HA",2) == 0)            return (char *)"H";            /* HA        */
    if (memcmp(buf,"BZ",2) == 0)            return (char *)"B";            /* BZIP2        */
    if (memcmp(buf,"\037\213",2) == 0)      return (char *)"G";            /* GZIP        */
    if (memcmp(buf,"\037\235",2) == 0)      return (char *)"C";            /* COMPRESS        */

    return NULL;        /* Unknown compressed or plain ASCII        */
}



static int searchlistfile(FILE ** someptr, const char *path,
                          char *foundfile, char *name, char *ext,
                          int unpackedonly);

/* Returns:
   -1 : error
    0 : not found
   >0 : found, next search for same list should start here
 */
int
openlist(FILE ** listFILEptr, char *filename, char *foundfile, int where,
         int mustbenew)
{
    int status;
    char ext[MYMAXEXT];
    char name[MYMAXFILE + MYMAXEXT];

    mklog(LOG_DEBUG, "openlist '%s', mustbenew %s", filename, mustbenew ? "yes":"no");

    myfnsplit(filename, NULL, NULL, name, ext);
    switch (where)
    {
    default:
        return 0;
    case SEARCH_UPLOAD:
        status =
            searchlistfile(listFILEptr, UploadDir, foundfile, name, ext,
                           0);
        if (status == 1)
            return SEARCH_MAILFILE;
        if (status != 0)
            return -1;
        /* FALLTHROUGH */
    case SEARCH_MAILFILE:
        status =
            searchlistfile(listFILEptr, MailfileDir, foundfile, name, ext,
                           0);
        if (status == 1)
            return SEARCH_UPDATE;
        if (status != 0)
            return -1;
        /* FALLTHROUGH */
    case SEARCH_UPDATE:
        if (mustbenew)
            return 0;
        status =
            searchlistfile(listFILEptr, UpdateDir, foundfile, name, ext,
                           1);
        if (status == 1)
            return SEARCH_MASTER;
        if (status != 0)
            return -1;
    case SEARCH_MASTER:
        if (mustbenew)
            return 0;
        status =
            searchlistfile(listFILEptr, MasterDir, foundfile, name, ext,
                           1);
        if (status == 1)
            return SEARCH_NOWHERE;
        if (status != 0)
            return -1;
        return 0;
    }
}

/* Returns:
   -1: error
    0: no matching file found
   +1: file found and opened */
int
searchlistfile(FILE ** file, const char *path, char *foundfile, char *name,
               char *ext, int unpackedonly)
{
    struct _filefind f;
    char fnamebuf[MYMAXDIR];
    char cmdlinebuf[128];
    char (*extptr)[MYMAXEXT];
    char extbuf[MYMAXEXT];
    char *findresult;
    int searchwhere;
    char *unarc;
    int i;
    char ArcOpen[ARCCMDMAX];

    mklog(LOG_DEBUG, "searchlistfile: '%s' '%s' '%s' '%s' %d", path, foundfile, name, ext, unpackedonly);

    if (path[0] == 0)
        return 0;
    while (!(ext[0] == 0 && unpackedonly))
    {        
        mklog(LOG_DEBUG, "searchlistfile(): in top of while loop");
        myfnmerge(foundfile, NULL, NULL, name, ext[0] ? ext : "*");
        findresult = os_findfile(&f, path, foundfile);
        if (!findresult)
        {
            mklog(LOG_DEBUG, "searchlistfile(): nothing found, return 0");
            foundfile[0] = '\0';
            return 0;
        }
        mklog(LOG_DEBUG, "searchlistfile(): found '%s'", findresult);
        getext(extbuf, findresult);
        myfnmerge(foundfile, NULL, path, findresult, NULL);
        os_deslashify(foundfile);
        if ((unarc = unpacker(foundfile)) != NULL)  /* Compressed file */
        {
            /* Search decompressor */
            ArcOpen[0] = '\0';
            for (i = 0; i < ArcOpenCnt; i++)
            {
                if (toupper((unsigned char)unarc[0]) == toupper((unsigned char)ArcOpenExt[i][0]))
                {
                    strcpy(ArcOpen, ArcOpenCmd[i]);
                    break;
                }
            }
            if (ArcOpen[0] == '\0')
            {
                mklog(LOG_ERROR, "No ArcOpen command for '%s'", foundfile);
            }
            else
            {
                mklog(LOG_INFO, "Attempting to unpack archive '%s'", foundfile);
            }
            myfnmerge(fnamebuf, NULL, path, NULL, NULL);
            os_deslashify(fnamebuf);
            /*
             * We need to chdir to the directory where the archive is found
             * so that the file is hopefully unpacked in the that directory.
             */
            if (chdir(fnamebuf))
            {
                mklog(LOG_ERROR, "Can't chdir to '%s'", fnamebuf);
            }
            else
            {
                /* Some archivers don't work well when the filename is given */
                /* sprintf(cmdlinebuf, "%s %s.%s", foundfile, name, ext); */
                sprintf(cmdlinebuf, "%s", foundfile);
                if ((ArcOpen[0] == '\0') || (os_spawn(ArcOpen, cmdlinebuf) != 0))
                {
                    mklog(LOG_ERROR, "Unable to unpack archive '%s'", foundfile);
                    WorkFile = os_file_getname(foundfile);
                    os_filecanonify(WorkFile);
                    *file = OpenMSGFile(NotifyAddress, NULL);
                    if (*file != NULL)
                    {
                        fprintf(*file, "Unable to unpack archive '%s'.\n", WorkFile);
                        fputs("Please resubmit it.", *file);
                        CloseMSGFile(1);
                    }
                }
                chdir(CurDir);
            }
            unlink(foundfile);
        }
        else if (ext[0] == 0 && toupper((unsigned char)extbuf[0]) == 'D') /* DIFFed 
                                                                             file 
                                                                           */
        {
            mklog(LOG_INFO, "Attempting to apply difference file '%s'", foundfile);
            extptr = OldExtensions;
            do
            {
                swapext(findresult, name, *extptr);
                searchwhere = SEARCH_UPDATE;
                while (searchwhere != 0)
                {
                    searchwhere =
                        openlist(file, findresult, fnamebuf, searchwhere,
                                 0);
                    if (searchwhere)
                    {
                        myfnmerge(fnamebuf, NULL, path, name,
                                  OldExtensions[0]);
                        if (ApplyDiff(*file, foundfile, fnamebuf) == 0)
                            goto out_of_loops;
                    }
                }
                extptr++;
            }
            while (extptr < OldExtensions + 3);
          out_of_loops:
            unlink(foundfile);
            if (searchwhere == 0)
            {
                mklog(LOG_INFO, "Unable to apply difference file '%s'", foundfile);
                WorkFile = os_file_getname(foundfile);
                os_filecanonify(WorkFile);
                *file = OpenMSGFile(NotifyAddress, NULL);
                if (*file)
                {
                    fprintf(*file, "Unable to apply difference file '%s'. ", WorkFile);
                    fprintf(*file, "Please submit your full update file.");
                    *file = CloseMSGFile(1);
                }
            }
        }
        else
        {
            goto justthisfile;
        }
    }
    for (extptr = OldExtensions; extptr < OldExtensions + 3; extptr++)
    {
        myfnmerge(foundfile, NULL, NULL, name, *extptr);
        if (os_findfile(&f, path, foundfile) != NULL)
        {
            myfnmerge(foundfile, NULL, path, name, *extptr);
            os_deslashify(foundfile);
justthisfile:
            mklog(LOG_DEBUG, "searchlistfile: justthisfile, foundfile='%s'", foundfile);
            *file = fopen(foundfile, "rb");
            if (!*file)
                return -1;
            WorkFile = os_file_getname(foundfile);
            os_filecanonify(WorkFile);
            return 1;
        }
    }
    return 0;
}

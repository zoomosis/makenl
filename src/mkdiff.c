/* $Id: mkdiff.c,v 1.10 2013/09/05 15:07:51 ozzmosis Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "makenl.h"
#include "fileutil.h"
#include "lsttool.h"
#include "mklog.h"

#ifdef __FLAT__
#define COLLTBLSIZE         ((65536 * 2) / 8)
#define COLLTBLBYTEPOS(idx) (idx >> 2)
#define COLLTBLBITPOS(idx)  ((idx & 3) << 1)
#define MAXHASHLISTENTRIES  16200
#define MINHASHLISTENTRIES  40
#else
#define COLLTBLSIZE         ((int)((32768L * 2) / 8))
#define COLLTBLBYTEPOS(idx) (idx >> 3)
#define COLLTBLBITPOS(idx)  (idx & 6)
#define MAXHASHLISTENTRIES  2050
#define MINHASHLISTENTRIES  20
#endif


#define ISHASH(entry)      (entry.hash < 0)
#define ISLINENO(entry)    (entry.hash >= 0)
#define ISCONNECTED(entry) ISLINENO(entry)

union _hashentry
{
    unsigned short hashlow;
    /* Make sure hash is allways 32 bits */
#if defined(__DOS16__) || defined(__OS2__)
    long hash;
#else
    int hash;
#endif
    int lineno;
};

struct DiffingInfo
{
    FILE *theFILE;
    int lineno;                 /* # of last read line from file */
    char *CollTbl;              /* hash collision table */
    union _hashentry *HashList; /* hash list */
    int hashentries;            /* current # entries in HashList */
};

static int maxhashes;           /* maximum entries in hash tables */

int CopyrightLines;

static int StillData;
static FILE *DiffFILE;
static struct DiffingInfo OldFile, NowFile;

/* Collision table handling                                                 */
/* The collision table has 32768 2-bit entries. Index into the table is     */
/* a 16-bit hash (in fact: only higher 15 bit of an unsigned short).        */
/* On 32-bit systems the collision table is 65536 entries with full 16-bit  */
/* index.                                                                   */
/* entry: 0=hash not used, 1=hash uniquely used, 2=hash collision           */

/* Get2Bit/Set2Bit: */
/* bitno:  the lower 16 bit of the hash value; index into the table         */
/* bitptr: pointer to the collision table                                   */

static int Get2Bit(char *bitptr, unsigned short bitno)
{
    return (bitptr[COLLTBLBYTEPOS(bitno)] >> COLLTBLBITPOS(bitno)) & 3;
}

static void Set2Bit(char *bitptr, unsigned short bitno, int val)
{
    bitptr[COLLTBLBYTEPOS(bitno)] &= (char) (~(3 << COLLTBLBITPOS(bitno)));
    bitptr[COLLTBLBYTEPOS(bitno)] |= (char) (val << COLLTBLBITPOS(bitno));
}

/* read line from source file until a new line is read. return this new line */

static int lineread(char *linebuf, int currentline, struct DiffingInfo *f)
{
    if (f->lineno > currentline)
        return 1;
    do
    {
        if (fgets(linebuf, linelength, f->theFILE) == NULL)
            return 0;
        if (linebuf[0] == '\032') /* Ignore EOF line */
            return 0;
        f->lineno++;
    }
    while (f->lineno <= currentline);
    return 1;
}

static long hashstr(const char *linebuf)
{
    long hashval = 0;

    while (*linebuf)
    {
        hashval <<= 1;
        if (hashval < 0)
            hashval |= 1;
        hashval ^= *linebuf;
        linebuf++;
    }
    return hashval | 0x80000000UL;
}

static int primebuffer(struct DiffingInfo *f, char *linebuf)
{
    unsigned short hashlow;
    long pos;

    memset(f->CollTbl, 0, COLLTBLSIZE);
    f->lineno = 0;
    f->hashentries = 0;

    pos = ftell(f->theFILE);
    while (lineread(linebuf, f->lineno, f))
    {
        if (f->hashentries >= maxhashes)
        {
            StillData = 1;
            break;
        }

        f->HashList[f->hashentries].hash = hashstr(linebuf);
        hashlow = f->HashList[f->hashentries].hashlow;
        f->hashentries++;
        switch (Get2Bit(f->CollTbl, hashlow))
        {
        case 0:
            Set2Bit(f->CollTbl, hashlow, 1);
            break;
        case 1:
            Set2Bit(f->CollTbl, hashlow, 2);
            break;
        }
    }
    fseek(f->theFILE, pos, SEEK_SET);
    f->lineno = 0;
    return f->hashentries;
}

static void WriteDiffPart(char *linebuf)
{
    int i;
    int idxnow, idxold;
    int aktline, linecount;
    char mybuf[linelength];

    /* Make the Copyright appear in the diff */
    /* if the copyright has more lines than the hashbuffer, then let only */
    /* as much lines appear as fit in the hash buffer.  */
    /* FIXED BUG from makenl 2.51: it crashes if the copyright has more */
    /* lines than the hashbuffer.  */
    if (CopyrightLines >= maxhashes)
        CopyrightLines = maxhashes - 1;

    for (i = 1; i <= CopyrightLines; i++)
    {
        if (ISCONNECTED(NowFile.HashList[i]))
            NowFile.HashList[i].hash = -1; /* remove connection */
    }
    CopyrightLines = 0;

    idxnow = idxold = 0;
    while (idxold < OldFile.hashentries && idxnow < NowFile.hashentries)
    {
        /* count matching lines, and do a real line compare on them */
        for (linecount = 0;
             idxold < OldFile.hashentries &&
             idxnow < NowFile.hashentries &&
             OldFile.HashList[idxold].lineno == idxnow;
             idxnow++, linecount++, idxold++)
        {
            lineread(mybuf, idxold, &OldFile);
            lineread(linebuf, idxnow, &NowFile);
            if (strcmp(mybuf, linebuf) != 0)
            {
                /* Remove false connections - after comparing input lines, 
                   you can be sure, whether the lines are equal */
                NowFile.HashList[idxnow].hash = -1;
                OldFile.HashList[idxold].hash = -1;
                break;
            }
        }
        if (linecount)
        {
            /* There is at least one copyable line */
            fprintf(DiffFILE, "C%d\r\n", linecount);
            continue;
        }

        /* Count unmatched lines in the old file, or lines matched against 
           some lines in the new file already written */
        linecount = 0;
        while (idxold < OldFile.hashentries &&
               (ISHASH(OldFile.HashList[idxold]) ||
                OldFile.HashList[idxold].lineno < idxnow))
        {
            linecount++;
            idxold++;
        }
        if (linecount)
        {
            /* Throw them away! */
            fprintf(DiffFILE, "D%d\r\n", linecount);
            continue;
        }

        /* Count lines in the new file that were not matched or matched
           against lines already read and copied/deleted */

        linecount = 0;
        while (idxnow < NowFile.hashentries &&
               (ISHASH(NowFile.HashList[idxnow]) ||
                NowFile.HashList[idxnow].lineno < idxold))
        {
            linecount++;
            idxnow++;
        }
        if (linecount)
        {
            /* There are such lines - put them into the diff */

            fprintf(DiffFILE, "A%d\r\n", linecount);
            aktline = idxnow - linecount;
            while (linecount--)
            {
                lineread(linebuf, aktline++, &NowFile);
                fputs(linebuf, DiffFILE);
            }
        }
        /* There seems to be a change in the order of the lines... Look
           which number is larger a) the numbers of line in the input till 
           the expected line comes OR b) the numbers of lines in the
           output, till the next input line appears */

        else if (NowFile.HashList[idxnow].lineno - idxold >=
                 OldFile.HashList[idxold].lineno - idxnow)
        {
            /* a) is larger - so emit the lines till the input line
               appears in output */
            linecount = OldFile.HashList[idxold].lineno - idxnow;
            fprintf(DiffFILE, "A%d\r\n", linecount);
            aktline = idxnow;
            for (; linecount != 0; linecount--)
            {
                lineread(linebuf, aktline++, &NowFile);
                fputs(linebuf, DiffFILE);
            }
            idxnow = OldFile.HashList[idxold].lineno;
        }
        else
        {
            /* b) is larger, so tell the reader to ignore input lines up
               to the matching line */

            fprintf(DiffFILE, "D%d\r\n",
                    NowFile.HashList[idxnow].lineno - idxold);
            idxold = NowFile.HashList[idxnow].lineno;
        }
    }

    /* this was not the last data block... Put the filepointers into
       correct position for next block */
    if (StillData)
    {
        NowFile.hashentries = idxnow;
        lineread(linebuf, OldFile.hashentries - 1, &OldFile);
        lineread(linebuf, NowFile.hashentries - 1, &NowFile);
        return;
    }

    /* we have to clean up... if there are lines left in the old file -
       DELETE them! */
    if (idxold < OldFile.hashentries)
        fprintf(DiffFILE, "D%d\r\n", OldFile.hashentries - idxold);

    /* if there are additional lines in the new file, they have to appear
       in the diff! */
    if (idxnow < NowFile.hashentries)
    {
        fprintf(DiffFILE, "A%d\r\n", NowFile.hashentries - idxnow);
        aktline = idxnow;
        for (; idxnow < NowFile.hashentries; idxnow++)
        {
            lineread(linebuf, aktline++, &NowFile);
            fputs(linebuf, DiffFILE);
        }
    }
}

int makediff(char *filename)
{
    char *run1, *run2;
    int foundConns;
    int synced;
    int i;
    char newext[MYMAXEXT];
    int cause;
    char oldname[MYMAXDIR];
    char diffname[MYMAXDIR];
    char linebuf[linelength];

    swapext(oldname, filename, OldExtensions[1]);
    if (os_filesize(filename) > DIFFThreshold  && DIFFThreshold != -1)
    {
        cause = CAUSE_THRESHOLD;
        strcpy(newext, OldExtensions[0]);
        newext[0] = 'd';        /* foo.206 --> foo.d06 */
        myfnmerge(diffname, NULL, OutDir, OutFile, newext);
    }
    else
    {
        if (OutDiff[0] == 0)
            return 0;
        cause = CAUSE_OUTDIFF;
        myfnmerge(diffname, NULL, OutDir, OutDiff, OldExtensions[0]);
    }

    OldFile.theFILE = fopen(oldname, "rb");
    if (!OldFile.theFILE)
    {
        mklog(LOG_ERROR, "Old file '%s' does not exist, no difference file made", oldname);
        return 0;
    }

    NowFile.theFILE = fopen(filename, "rb");
    if (!NowFile.theFILE)
        die(254, "Unable to open new node list -- '%s'\n", filename);

    DiffFILE = fopen(diffname, "wb");
    if (!DiffFILE)
        die(254, "Unable to create difference file -- '%s'\n", diffname);

    /* skip first line of new file */
    if (!fgets(linebuf, linelength, NowFile.theFILE))
    {
        fclose(OldFile.theFILE);
        fclose(NowFile.theFILE);
        fclose(DiffFILE);
        unlink(diffname);
        return 0;
    }

    /* get first line of old file */
    if (!fgets(linebuf, linelength, OldFile.theFILE))
    {
        fclose(OldFile.theFILE);
        fclose(NowFile.theFILE);
        fclose(DiffFILE);
        unlink(diffname);
        return 0;
    }
    fputs(linebuf, DiffFILE);

    mklog(LOG_INFO, "Creating difference file '%s' from '%s' and '%s'",
      diffname, oldname, filename);

    /* allocate buffer memory for diff-hashing */
    OldFile.CollTbl = NULL;
    maxhashes = MAXHASHLISTENTRIES;

    while (maxhashes >= MINHASHLISTENTRIES)
    {
        OldFile.CollTbl =
            malloc(2 * COLLTBLSIZE + 2 * maxhashes * sizeof(long));
        if (OldFile.CollTbl)
            break;
        else                    /* no memory: */
            maxhashes -= 100;   /* shorten hash table by 100 entries */
    }
    if (!OldFile.CollTbl)
    {
        mklog(LOG_ERROR, "Unable to allocate memory -- no difference file generated");
        fclose(OldFile.theFILE);
        fclose(NowFile.theFILE);
        fclose(DiffFILE);
        unlink(diffname);
        return 0;
    }

    /* order of buffers in allocated memory: */
    /* OldFile.CollTbl, NowFile.CollTbl, OldFile.HashList,
       NowFile.HashList */
    NowFile.CollTbl = OldFile.CollTbl + COLLTBLSIZE;
    OldFile.HashList =
        (union _hashentry *)((char *)NowFile.CollTbl + COLLTBLSIZE);
    NowFile.HashList = OldFile.HashList + maxhashes;


    /* begin diff process */
    fseek(OldFile.theFILE, 0L, SEEK_SET);
    fseek(NowFile.theFILE, 0L, SEEK_SET);
    do
    {
        int idxold;
        int idxnow;

        StillData = 0;
        primebuffer(&NowFile, linebuf);
        primebuffer(&OldFile, linebuf);
        run1 = OldFile.CollTbl;
        run2 = NowFile.CollTbl;
        for (i = 0; i < COLLTBLSIZE; i++)
            *(run1++) &= *(run2++);

        /* Step 1: connect all lines with unique hash value */
        idxnow = idxold = 0;
        while (idxold < OldFile.hashentries) /* Walk old file line-by-line 
                                              */
        {
            if (idxnow < NowFile.hashentries) /* try walking in new file
                                                 also */
                idxnow++;

            if (Get2Bit(OldFile.CollTbl, OldFile.HashList[idxold].hashlow)
                == 1)
            {
                int searchforw = idxnow;
                int searchback = idxnow;

                /* If the hash value is unique in both files... */
                /* ...search the line in the new file */
                while (searchforw < NowFile.hashentries || searchback > 0)
                {
                    if (searchforw < NowFile.hashentries &&
                        NowFile.HashList[searchforw++].hash ==
                        OldFile.HashList[idxold].hash)
                    {
                        idxnow = searchforw - 1;
                        NowFile.HashList[idxnow].hash = 0;
                        OldFile.HashList[idxold].hash = 0;
                        NowFile.HashList[idxnow].lineno = idxold;
                        OldFile.HashList[idxold].lineno = idxnow;
                        break;
                    }
                    if (searchback > 0 &&
                        NowFile.HashList[--searchback].hash ==
                        OldFile.HashList[idxold].hash)
                    {
                        idxnow = searchback;
                        NowFile.HashList[idxnow].hash = 0;
                        OldFile.HashList[idxold].hash = 0;
                        NowFile.HashList[idxnow].lineno = idxold;
                        OldFile.HashList[idxold].lineno = idxnow;
                        break;
                    }
                }
            }
            idxold++;
        }

        /* Step 2: Walk through the lines of the old file. If you hit an
           unconnected line after connected lines, test whether in both
           files the lines have the same hash value (it need not to be
           unique, we just guess that lines with a non-unique hash value
           after connected lines are also equal. In a second iteration,
           connect lines before already connected lines. */

        /* Step 2a: go forward */
        synced = 1;
        idxold = idxnow = -1;
        while (++idxold < OldFile.hashentries) /* Again, walk old file
                                                  step by step */
        {
            /* This line connected to a line in the new file? */
            if (ISCONNECTED(OldFile.HashList[idxold]))
            {
                synced = 1;     /* if yes, resynchronize... */
                idxnow = OldFile.HashList[idxold].lineno;
            }
            else if (synced)
            {
                /* Do the hashes match? */
                if (++idxnow < NowFile.hashentries &&
                    NowFile.HashList[idxnow].hash ==
                    OldFile.HashList[idxold].hash)
                {
                    /* Yes, connect the lines! */
                    OldFile.HashList[idxold].hash = 0;
                    NowFile.HashList[idxnow].hash = 0;
                    OldFile.HashList[idxold].lineno = idxnow;
                    NowFile.HashList[idxnow].lineno = idxold;
                }
                else
                    /* No, fell out of sync */
                    synced = 0;
            }
        }

        /* Step 2b: Now do the same thing again backwards, and test,
           whether there are any connections - we need this information
           later */

        synced = 1;
        foundConns = 0;
        idxold = OldFile.hashentries;
        idxnow = NowFile.hashentries;
        while (--idxold >= 0)
        {
            if (ISCONNECTED(OldFile.HashList[idxold]))
            {
                foundConns = 1;
                synced = 1;
                idxnow = OldFile.HashList[idxold].lineno;
            }
            else if (synced)
            {
                if (--idxnow >= 0 &&
                    NowFile.HashList[idxnow].hash ==
                    OldFile.HashList[idxold].hash)
                {
                    OldFile.HashList[idxold].hash = 0;
                    NowFile.HashList[idxnow].hash = 0;
                    OldFile.HashList[idxold].lineno = idxnow;
                    NowFile.HashList[idxnow].lineno = idxold;
                }
                else
                    synced = 0;
            }
        }

        /* If you found connections, try in the next iteration with
           datablocks directly after the last connection. */

        if (StillData && foundConns)
        {
            while (!ISCONNECTED(OldFile.HashList[--OldFile.hashentries]))
                ;
            OldFile.hashentries++;
            while (!ISCONNECTED(NowFile.HashList[--NowFile.hashentries]))
                ;
            NowFile.hashentries++;
        }

        WriteDiffPart(linebuf);
    }
    while (StillData);
    free(OldFile.CollTbl);      /* The start of the memory area */
    fclose(OldFile.theFILE);
    fclose(NowFile.theFILE);
    fclose(DiffFILE);
    if (cause == CAUSE_THRESHOLD)
    {
        strcpy(filename, diffname);
        if (OutDiff[0] != 0)
        {
            cause = CAUSE_THRESHOLD | CAUSE_OUTDIFF;
            CopyOrMove(1, diffname, OutDir, OutDiff);
        }
    }
    return cause;
}

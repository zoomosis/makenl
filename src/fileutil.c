/* $Id: fileutil.c,v 1.8 2012/11/13 22:38:44 ozzmosis Exp $ */

#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "makenl.h"
#include "config.h"
#include "fileutil.h"
#include "mklog.h"

#ifdef MALLOC_DEBUG
#include "rmalloc.h"
#endif

#ifdef DMALLOC
#include "dmalloc.h"
#endif

char OldExtensions[4][MYMAXEXT];
int do_clean;

char MakeSourceFile[MYMAXFILE + MYMAXEXT];
char OutFile[MYMAXFILE + MYMAXEXT];
char OutDiff[MYMAXFILE + MYMAXEXT];
char CopyrightFile[MYMAXFILE + MYMAXEXT] = "cpyright.txt";
char PrologFile[MYMAXFILE + MYMAXEXT] = "prolog.txt";
char EpilogFile[MYMAXFILE + MYMAXEXT] = "epilog.txt";
char MergeFilename[MYMAXPATH];
char CommentsFile[MYMAXPATH];

char CurDir[MYMAXDIR];
char OutDir[MYMAXDIR];
char MasterDir[MYMAXDIR];
char UpdateDir[MYMAXDIR];
char MessageDir[MYMAXDIR];
char MailfileDir[MYMAXDIR];
char UploadDir[MYMAXDIR];
char BadDir[MYMAXDIR];

char BatchFile[MYMAXPATH];
char CalledBatchFile[MYMAXFILE];
char LogFile[MYMAXPATH];

int GetPath(char *arg, int switchno)
{
    char *destptr;

    switch (switchno)
    {
    default:
        return 0;
    case CFG_BADFILES:
        destptr = BadDir;
        break;
    case CFG_MAILFILES:
        destptr = MailfileDir;
        break;
    case CFG_MASTER:
        destptr = MasterDir;
        break;
    case CFG_MESSAGES:
        destptr = MessageDir;
        break;
    case CFG_OUTPATH:
        destptr = OutDir;
        break;
    case CFG_UPDATE:
        destptr = UpdateDir;
        break;
    case CFG_UPLOADS:
        destptr = UploadDir;
        break;
    }
    return (os_fulldir(destptr, arg, MYMAXDIR) == 0);
}

int getext(char *ext, char *filename) /* returns length of extension 0..3 */
{
    char extbuf[MYMAXEXT], *extptr;

    if (ext)
        extptr = ext;
    else
        extptr = extbuf;
    myfnsplit(filename, NULL, NULL, NULL, extptr);
    return strlen(extptr);
}

void swapext(char *newname, const char *origname, const char *newext)
{
    char drive[MYMAXDRIVE];
    char name[MYMAXFILE];
    char path[MYMAXDIR];

    myfnsplit(origname, drive, path, name, NULL);
    myfnmerge(newname, drive, path, name, newext);
}

void
myfnmerge(char *output, const char *drive, const char *dir,
          const char *name, const char *ext)
{
    int lenleft;

    mklog(LOG_DEBUG, "myfnmerge: drive='%s' dir='%s' name='%s' ext='%s'", 
      make_str_safe(drive), make_str_safe(dir), make_str_safe(name), make_str_safe(ext));

    lenleft = MYMAXDIR - 1;
    if (drive && *drive != 0)
    {
        *(output++) = *drive;
        *(output++) = ':';
        lenleft = MYMAXDIR - 3;
    }
    if (dir && *dir)
    {
        while (lenleft && *dir)
        {
            *(output++) = *(dir++);
            lenleft--;
        }
        if (lenleft && (*(output - 1) != '\\' && *(output - 1) != '/'))
        {
            *(output++) = CHAR_DIRSEPARATOR;
            lenleft--;
        }
    }
    if (name && *name)
    {
        while (lenleft && *name)
        {
            *(output++) = *(name++);
            lenleft--;
        }
    }
    if (ext && *ext)
    {
        if (lenleft && *(output - 1) != '.')
        {
            *(output++) = '.';
            lenleft--;
        }
        if (*ext == '.')
            ext++;
        while (lenleft && *ext)
        {
            *(output++) = *(ext++);
            lenleft--;
        }
    }
    *output = 0;
}

void
myfnsplit(const char *input, char *drive, char *dir, char *name, char *ext)
{
    const char *splitptr;
    const char *splitptr2;
    int lenleft;

    if (input[1] == ':')
    {
        if (drive)
        {
            drive[0] = input[0];
            drive[1] = 0;
        }
        input += 2;
    }
    else if (drive)
        *drive = 0;
    splitptr = strrchr(input, '\\');
    splitptr2 = strrchr(input, '/');
    if (splitptr2 > splitptr)
        splitptr = splitptr2;
    if (!splitptr)
        splitptr = input - 1;
    if (dir)
    {
        for (lenleft = MYMAXDIR - 1; input <= splitptr && lenleft;
             lenleft--)
            *(dir++) = *(input++);

        if (lenleft < MYMAXDIR - 2) /* the single '\' may *not* be deleted 
                                     */
            dir--;              /* delete trailing '\' */

        *dir = 0;
    }
    else
        input = splitptr + 1;
    splitptr = strchr(input, '.');
    if (!splitptr)
        splitptr = strchr(input, 0);
    if (name)
    {
        for (lenleft = MYMAXFILE - 1; input < splitptr && lenleft;
             lenleft--)
            *(name++) = *(input++);
        *name = 0;
    }
    if (ext)
    {
        if (*splitptr)
            splitptr++;         /* Skip the dot */
        for (lenleft = 3; *splitptr && lenleft; lenleft--)
            *(ext++) = *(splitptr++);
        *ext = 0;
    }
}

long os_filesize(const char *filename)
{
    long size;
    int handle;

    handle = open(filename, O_RDONLY);
    if (handle == -1)
        return -1;
    size = lseek(handle, 0, SEEK_END);
    close(handle);
    if (size < 0)
        return -1;
    else
        return size;
}

void cleanold(char *path, char *filename, char *ext)
{
    char *nameptr;
    struct _filefind f;
    char fnamebuf[MYMAXDIR];

    if (path[0] == 0)
        return;
    if (getext(NULL, filename))
    {
        /* extension in filename given: delete exactly that file */
        myfnmerge(fnamebuf, NULL, path, filename, NULL);
        unlink(fnamebuf);
    }
    else if (ext && ext[0] != 0)
    {
        /* no extension in filename but extension in ext: delete
           filename+ext */
        myfnmerge(fnamebuf, NULL, path, filename, ext);
        unlink(fnamebuf);
    }
    else
    {
        /* no extension in filename or ext: delete filename.* */
        myfnmerge(fnamebuf, NULL, NULL, filename, "*");

        for (nameptr = os_findfirst(&f, path, fnamebuf);
             nameptr != NULL; nameptr = os_findnext(&f))
        {
            myfnmerge(fnamebuf, NULL, path, nameptr, NULL);
            unlink(fnamebuf);
        }
        os_findclose(&f);
    }
}

void cleanfile(char *filename)
{
    mklog(LOG_DEBUG, __FILE__ ": cleanfile(): delete %s", filename);
    if (unlink(filename) == 0)
    {
        mklog(LOG_INFO, __FILE__ ": cleanfile(): deleted '%s'", filename);
    }
}

void cleanit(void)
{
    char (*extptr)[MYMAXEXT];
    char ext[MYMAXEXT];
    char delname[MYMAXDIR];

    mklog(LOG_DEBUG, __FILE__ ": cleanit(): cleanup %s", do_clean ? "yes":"no");
    if (!do_clean)
        return;
    if (getext(NULL, OutFile) != 0)
        return;
    extptr = OldExtensions + 1;
    do
    {
        strcpy(ext, *extptr);
        if (OutDiff[0] != 0)
        {
            myfnmerge(delname, NULL, OutDir, OutDiff, ext);
            cleanfile(delname);
            ext[0] = ArcCopyExt[0];
            myfnmerge(delname, NULL, OutDir, OutDiff, ext);
            cleanfile(delname);
        }
        ext[0] = 'd';
        myfnmerge(delname, NULL, OutDir, OutFile, ext);
        cleanfile(delname);
        ext[0] = ArcCopyExt[0];
        myfnmerge(delname, NULL, OutDir, OutFile, ext);
        cleanfile(delname);
        ext[1] = 'd';
        myfnmerge(delname, NULL, OutDir, OutFile, ext);
        cleanfile(delname);
        extptr++;
    }
    while (extptr < OldExtensions + 4);
}

void CopyOrMove(int copy, char *source, char *destdir, char *destname)
{
    char dest[MYMAXDIR];
    int copychar;               /* Yes *int* and *char* are intentionally! 
                                 */
    FILE *destFILE;             /* 0x04 */
    FILE *sourceFILE;           /* 0x06 */

    if (destname[0] == 0)
        return;

    myfnmerge(dest, NULL, destdir, destname,
              getext(NULL, destname) ? NULL : OldExtensions[0]);
    if (!filecmp(dest, source))
        return;
    mklog(LOG_INFO, "%sing '%s' to '%s'", copy ? "Copy" : "Mov", source, dest);
    unlink(dest);
    if (!copy && rename(source, dest) == 0)
        return;
    destFILE = fopen(dest, "w");
    if (!destFILE)
        die(254, "Unable to open '%s' for output", dest);
    sourceFILE = fopen(source, "r");
    if (!sourceFILE)
        die(254, "Unable to open '%s' for input", source);
    while ((copychar = getc(sourceFILE)) != EOF)
        fputc(copychar, destFILE);
    fclose(sourceFILE);
    fclose(destFILE);
    if (!copy)
        unlink(source);
}

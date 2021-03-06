#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "os.h"

#if defined(OS_UNIX) || defined(__DJGPP__) || defined(__EMX__)
#include <dirent.h>
#ifdef __linux__
/* _GNU_SOURCE needed for FNM_CASEFOLD */
#define _GNU_SOURCE
#endif
#include <fnmatch.h>
#endif

#ifdef __TURBOC__
#include <dos.h>
#endif

#ifdef __BORLANDC__
#include <direct.h>
#include <io.h>
#endif

#include "fileutil.h"
#include "snprintf.h"
#include "strtool.h"
#include "mklog.h"
#include "unused.h"

char *os_file_getname(const char *path)
{
#ifdef __EMX__
    return _getname(path);
#else
    const char *p;

    p = path;

    while (*path != 0)
    {
        switch (*path++)
        {
        case ':':
        case '/':
        case '\\':
            p = path;           /* Note that PATH has been incremented */
            break;
        }
    }

    return (char *)p;
#endif
}

/*
 *  os_chdir()
 *
 *  Workaround for systems where chdir() will not accept a trailing \ character unless
 *  it's the root of the drive.
 */

#if defined(OS_DOS) || defined(OS_OS2) || defined(OS_WIN)

static char driveletters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static int isvaliddriveletter(int drive)
{
    return strchr(driveletters, toupper(drive)) != NULL;
}

static int drvlet2num(char drvletter)
{
    return (int) (strchr(driveletters, toupper(drvletter)) - driveletters + 1);
}

int os_getdrive(void)
{
    /* Returns the current drive number (A:=1, B:=2, etc.) */
    #if defined (__EMX__)
    return drvlet2num(_getdrive());
    #elif defined (__DJGPP__) || defined (__TURBOC__)
    return getdisk() + 1; /* DJGPP/TC getdisk() drive numbers are 0 based. */
    #elif defined (__WATCOMC__)
    unsigned drive;
    _dos_getdrive(&drive);
    return drive;
    #else
    return _getdrive();
    #endif
}

int os_chdrive(int newdrv)
{
    /* EMX _chdrive() needs drive letter instead of number */
    #if defined (__EMX__)
    char reqdrv;
    reqdrv = driveletters[newdrv - 1];
    _chdrive(reqdrv);
    return (_getdrive() != reqdrv); /* Check to make sure disk change
                                       succeeded - return 0 for success
                                       or 1 for failure. */

    #elif defined (__DJGPP__) || (defined (__TURBOC__) && !defined (__BORLANDC__))
    setdisk(newdrv - 1);
    return (getdisk() != (newdrv - 1)); /* Check to make sure disk change
                                           succeeded - return 0 for success
                                           or 1 for failure. */

    #elif defined (__WATCOMC__)
    unsigned drive, total;
    _dos_setdrive(newdrv, &total);
    _dos_getdrive(&drive);
    return (drive != newdrv);

    #else
    return _chdrive(newdrv);
    #endif
}


int os_chdir(char *path)
{
    char *newpath;
    int rc;
    int curdrv;
    int newdrv;

    mklog(LOG_DEBUG, "os_chdir(): path='%s'", path);

    if (path == NULL || path[0] == '\0')
    {
        /* null pointer or empty string; do nothing */
        return 0;
    }

    if (path[0] == '\\' && path[1] == '\0')
    {
        /* lone '\' path is OK */
        return chdir(path);
    }
    /* Get the current drive */
    curdrv = os_getdrive();
    mklog(LOG_DEBUG, "os_chdir(): curdrv='%d' (%c:)", curdrv, driveletters[curdrv - 1]);

    if (isvaliddriveletter(path[0]) && path[1] == ':' && path[2] == '\\' && path[3] == '\0')
    {
        /* "x:\" is OK too, but we need to check if we are changing drives. */

        if (drvlet2num(path[0]) != curdrv)
        {
            /* Path specified is not on current drive. */

            newdrv = drvlet2num(path[0]);
            mklog(LOG_DEBUG, "os_chdir(): newdrv='%d' (%c:)", newdrv, driveletters[curdrv - 1]);
            if (os_chdrive(newdrv))
            {
                /* Failed to change drives! */

                mklog(LOG_DEBUG, "os_chdir(): os_chdrive('%d') failed!", newdrv);
                return 1;
            }
        }
        return chdir(path);
    }

    /* copy path and modify it */

    newpath = malloc(strlen(path) + 1);

    if (newpath == NULL)
    {
        /* out of memory */
        return 1;
    }

    strlcpy(newpath, path, strlen(path) + 1);

    /* strip trailing \ */
    os_remove_slash(newpath);

    mklog(LOG_DEBUG, "os_chdir(): newpath='%s'", newpath);

    /* Check if we are changing drives. */

    if (isvaliddriveletter(newpath[0]) && newpath[1] == ':')
    {
        /* Drive letter at start of path. */

        if (drvlet2num(newpath[0]) != curdrv)
        {
            /* We are changing drives. */

            newdrv = drvlet2num(newpath[0]);
            mklog(LOG_DEBUG, "os_chdir(): newdrv='%d' (%c:)", newdrv, driveletters[newdrv - 1]);
            if (os_chdrive(newdrv))
            {
                /* Failed to change drives! */

                mklog(LOG_DEBUG, "os_chdir(): os_chdrive('%d') failed!", newdrv);
                return 1;
            }
        }
    }

    rc = chdir(newpath);

    if (rc != 0)
    {
        mklog(LOG_ERROR, "$Can't chdir to '%s'", newpath);
    }

    free(newpath);

    return rc;
}

#else

int os_chdir(char *path)
{
    int rc;

    rc = chdir(path);

    if (rc != 0)
    {
        mklog(LOG_ERROR, "$Can't chdir to '%s'", path);
    }

    return rc;
}

#endif

/*
 * os_append_slash()
 *
 * It is assumed that path has enough room for the slash.
 */

char *os_append_slash(char *path)
{
    char *p;

    if (path == NULL)
    {
        return NULL;
    }

    p = strchr(path, '\0');

    if (p > path)
    {
        p--;

        if (*p != '\\' && *p != '/')
        {
            *(++p) = DIRSEP[0];
            *(++p) = '\0';
        }
    }

    return path;
}


/*
 * os_remove_slash()
 *
 * Remove slash/backslash from last part of path name, if present.
 */

char *os_remove_slash(char *path)
{
    char *new_path, *p;

    if (path == NULL)
    {
        return NULL;
    }

    new_path = path;

#if defined(OS_DOS) || defined(OS_OS2) || defined(OS_WIN)
    if (*new_path && new_path[1] == ':')
    {
        /* skip drive letter */
        new_path += 2;
    }
#endif

    p = strchr(new_path, '\0');

    if (p > new_path)
    {
        p--;

        if (p > new_path && (*p == '\\' || *p == '/'))
        {
            *p = '\0';
        }
    }

    return path;
}


/*
 * os_dirsep()
 *
 * On DOS, OS/2 & Windows systems this converts / slashes to \ backslashes
 * in pathnames. On other systems it does nothing.
 */

char *os_dirsep(char *path)
{
#if defined(OS_DOS) || defined(OS_OS2) || defined(OS_WIN)
    char *p;

    if (path == NULL)
    {
        return NULL;
    }

    p = path;

    while (*p != 0)
    {
        if (*p == '/')
        {
            *p = '\\';
        }
        p++;
    }
#endif

    return path;
}

int os_spawn(const char *command, const char *cmdline)
{
#ifdef __EMX__
    char execfn[_MAX_PATH];
    char tmpfn[_MAX_PATH];
    char *pext;
    char *cmd;
    int bufsize;
    int rc;

    /* search for command */
    strlcpy(tmpfn, command, sizeof tmpfn);
    pext = strchr(tmpfn, '\0');

    mklog(LOG_DEBUG, "os_spawn(): trying '%s'", tmpfn);
    rc = _path(execfn, tmpfn);

    if (rc != 0)
    {
        strcpy(pext, ".EXE");
        mklog(LOG_DEBUG, "os_spawn(): trying '%s'", tmpfn);
        rc = _path(execfn, tmpfn);
    }
    else if (rc != 0 && _osmode == OS2_MODE)
    {
        strcpy(pext, ".CMD");
        mklog(LOG_DEBUG, "os_spawn(): trying '%s'", tmpfn);
        rc = _path(execfn, tmpfn);
    }
    else if (rc != 0 && _osmode != OS2_MODE)
    {
        strcpy(pext, ".COM");
        mklog(LOG_DEBUG, "os_spawn(): trying '%s'", tmpfn);
        rc = _path(execfn, tmpfn);
    }
    else if (rc != 0 && _osmode != OS2_MODE)
    {
        strcpy(pext, ".BAT");
        mklog(LOG_DEBUG, "os_spawn(): trying '%s'", tmpfn);
        rc = _path(execfn, tmpfn);
    }
    else if (rc != 0)
    {
        mklog(LOG_ERROR, "os_spawn(): program not found");
        return -1;
    }

    bufsize = strlen(command) + 1 + strlen(cmdline) + 1;
    cmd = malloc(bufsize);

    if (!cmd)
    {
        return -1;
    }

    snprintf(cmd, bufsize, "%s %s", command, cmdline);
    mklog(LOG_DEBUG, "found: executing '%s'", cmd);
    rc = system(cmd);
    mklog(LOG_DEBUG, "os_spawn() rc=%d", rc);

    free(cmd);
    return rc;
#else
    char *cmd;
    int rc;
    int bufsize;

    bufsize = strlen(command) + 1 + strlen(cmdline) + 1;
    cmd = malloc(bufsize);

    if (!cmd)
    {
        mklog(LOG_ERROR, "os_spawn(): out of memory for command line buffer");
        return -1;
    }

    snprintf(cmd, bufsize, "%s %s", command, cmdline);
    mklog(LOG_DEBUG, "os_spawn(): '%s'", cmd);
    rc = system(cmd);
    mklog(LOG_DEBUG, "os_spawn(): system() rc=%d", rc);

    free(cmd);
    return rc;
#endif
}

char *os_findfile(struct _filefind *pff, const char *path, const char *mask)
{
    char *p;

    p = os_findfirst(pff, path, mask);

    if (p != NULL)
    {
        strcpy(pff->path, p);
        os_findclose(pff);
        return pff->path;
    }

    return NULL;
}

/*
 * os_fulldir()
 *
 * Make an absolute path from given relative path.
 * src should specify an existing directory name.
 */

int os_fulldir(char *dst, const char *src, size_t bufsiz)
{
    char tmp[MYMAXPATH];
    struct stat st;
    int rc;

    mklog(LOG_DEBUG, __FILE__ ": os_fulldir(): called with src='%s'", src);

    strlcpy(tmp, src, sizeof tmp);
    os_remove_slash(tmp);
    mklog(LOG_DEBUG, __FILE__ ": os_fulldir(): after removing slash: '%s'", tmp);

    if (os_fullpath(dst, tmp, bufsiz) != 0)
    {
        mklog(LOG_DEBUG, __FILE__ ": os_fulldir(): os_fullpath() failed!");
        return -1;
    }

    mklog(LOG_DEBUG, __FILE__ ": os_fulldir(): absolute path '%s'", dst);

    rc = stat(dst, &st);

    if (rc != 0)
    {
        mklog(LOG_DEBUG, __FILE__ ": os_fulldir(): stat('%s') rc=%d!", dst, rc);
        return -1;
    }

    mklog(LOG_DEBUG, __FILE__ ": os_fulldir(): st_mode is now %o", st.st_mode);

#ifdef __TURBOC__
#ifndef S_ISDIR
#define S_ISDIR(m)  ((m) & S_IFDIR)
#endif
    return S_ISDIR(st.st_mode) ? 0 : -1;
#elif defined(__IBMC__)
    return st.st_mode & S_IFDIR ? 0 : -1;
#else
    return ((st.st_mode & S_IFMT) == S_IFDIR) ? 0 : -1;
#endif
}

#if defined(OS_UNIX)

char *os_fgets(char *buffer, size_t len, FILE * f)
{
    char *result;

    result = fgets(buffer, len, f);

    /*
     * If we see an EOF at the beinning of a line, then make it look as
     * if we didn't read anything.
     */

    if (result != NULL && buffer[0] == ('Z' & 0x1F))
    {
        return NULL;
    }

    /*
     * This should not happen, EOF should always be at the beginning of a line.
     */

    if (result != NULL && buffer[strlen(buffer) - 1] == ('Z' & 0x1F))
    {
        buffer[strlen(buffer) - 1] = 0;
    }

    return result;
}

#else

char *os_fgets(char *buffer, size_t len, FILE * f)
{
    return fgets(buffer, len, f);
}

#endif

#if defined(OS_UNIX) || defined(__DJGPP__) || defined(__EMX__)

char *os_findfirst(struct _filefind *pff, const char *path,
                   const char *mask)
{
    strcpy(pff->path, path);
    os_remove_slash(pff->path);
    strcpy(pff->mask, mask);
    pff->flags = FNM_NOESCAPE;

#if defined(__EMX__) && defined(_FNM_OS2)
    pff->flags |= (_osmode == OS2_MODE) ? _FNM_OS2 : _FNM_DOS;
    pff->flags |= _FNM_IGNORECASE;
#endif

#ifdef __DJGPP__
    pff->flags |= FNM_CASEFOLD;
#endif

    pff->dirp = opendir(pff->path);

    if (pff->dirp != NULL)
    {
        char *p;

        p = os_findnext(pff);
        if (p != NULL)
        {
            return p;
        }
    }

    closedir(pff->dirp);
    pff->dirp = NULL;
    return NULL;
}

char *os_findnext(struct _filefind *pff)
{
    int matchresult;

    while (1)
    {
        pff->pentry = readdir(pff->dirp);

        if (pff->pentry == NULL)
        {
            return NULL;
        }

        matchresult = fnmatch(pff->mask, pff->pentry->d_name, pff->flags);

        if (matchresult == 0)
        {
            return pff->pentry->d_name;
        }
    }
}

void os_findclose(struct _filefind *pff)
{
    if (pff->dirp)
    {
        closedir(pff->dirp);
    }

    pff->dirp = NULL;
}

#endif

char *os_getcwd(char *buf, size_t size)
{
#ifdef __EMX__
    return _getcwd2(buf, size);
#else
    return getcwd(buf, size);
#endif
}

#ifdef OS_UNIX

int os_fullpath(char *dst, const char *src, size_t bufsize)
{
    char olddir[MYMAXDIR];
    char dir[MYMAXDIR];
    char name[MYMAXFILE];
    char ext[MYMAXEXT];
    int rc;

    myfnsplit(src, NULL, dir, name, ext);
    mklog(LOG_DEBUG, "os_fullpath(): dir='%s', name='%s', ext='%s'", dir, name, ext);

    if (getcwd(olddir, sizeof olddir) == NULL)
    {
        mklog(LOG_ERROR, "os_fullpath(): getcwd() failed!");
        return -1;
    }

    mklog(LOG_DEBUG, "os_fullpath(): old directory='%s'", olddir);

    if (dir[0] && os_chdir(dir) != 0)
    {
        mklog(LOG_ERROR, "os_fullpath(): chdir() to '%s' failed!", dir);
        return -1;
    }

    if (getcwd(dir, MYMAXDIR) == NULL || strlen(dir) + strlen(name) + strlen(ext) > bufsize)
    {
        mklog(LOG_ERROR, "os_fullpath(): Directory name for '%s' too long!", src);

        rc = os_chdir(olddir);

        if (rc != 0)
        {
            mklog(LOG_ERROR, "os_fullpath(): chdir() to '%s' failed!", olddir);
        }

        return -1;
    }

    myfnmerge(dst, NULL, dir, name, ext);
    mklog(LOG_DEBUG, "os_fullpath(): complete filename: '%s'", dst);

    rc = os_chdir(olddir);

    if (rc != 0)
    {
        mklog(LOG_ERROR, "os_fullpath(): chdir() to '%s' failed!", olddir);
        return -1;
    }

    return 0;
}

#endif

#if defined(OS_DOS) || defined(OS_OS2) || defined(OS_WIN)

#if defined(__WATCOMC__) || (defined(OS_OS2) && (defined(__BORLANDC__) || defined(__HIGHC__) || defined(__IBMC__)))

#if defined(OS_OS2) && defined(__IBMC__)

#define INCL_BASE
#define INCL_DOS
#include <os2.h>

/* file attribute constants for attr field of _dos_findfirst() */

#define _A_NORMAL 0x00    /* normal file, read/write permitted */
#define _A_RDONLY 0x01    /* read-only file */
#define _A_HIDDEN 0x02    /* hidden file */
#define _A_SYSTEM 0x04    /* system file */
#define _A_VOLID  0x08    /* volume label */
#define _A_SUBDIR 0x10    /* subdirectory */
#define _A_ARCH   0x20    /* archive file */

#define BAD_HANDLE         ((HDIR)(~0))
#define HANDLE_OF(__find)  (*(HDIR *)(&(__find)->reserved[0]))

#define FF_LEVEL   1
#define FF_BUFFER  FILEFINDBUF3

struct name
{
    char buf[NAME_MAX + 1];
};

static void copydir(struct find_t *buf, FF_BUFFER *dir_buff)
{
    buf->attrib = dir_buff->attrFile;
    buf->wr_time = *(unsigned short *)&dir_buff->ftimeLastWrite;
    buf->wr_date = *(unsigned short *)&dir_buff->fdateLastWrite;
    buf->size = dir_buff->cbFile;

    *(struct name *)buf->name = *(struct name *)dir_buff->achName;
}

static unsigned _dos_findfirst( const char *path, unsigned attr, struct find_t *buf)
{
    APIRET rc;
    FF_BUFFER dir_buff;
    HDIR handle = BAD_HANDLE;
    ULONG searchcount;

    searchcount = 1;        /* only one at a time */

    rc = DosFindFirst((PSZ)path, (PHFILE)&handle, attr, (PVOID)&dir_buff, sizeof dir_buff, &searchcount, FF_LEVEL);

    if (rc != 0 && rc != ERROR_EAS_DIDNT_FIT)
    {
        HANDLE_OF(buf) = BAD_HANDLE;
        errno = ENOENT;
        return 1;
    }

    HANDLE_OF(buf) = handle;
    copydir(buf, &dir_buff);      /* copy in other fields */

    return 0;
}

static unsigned _dos_findnext( struct find_t *buf )
{
    APIRET rc;
    FF_BUFFER dir_buff;
    ULONG searchcount = 1;

    rc = DosFindNext(HANDLE_OF(buf), (PVOID)&dir_buff, sizeof dir_buff, &searchcount);

    if (rc != 0)
    {
        errno = ENOENT;
        return 1;
    }

    copydir( buf, &dir_buff );

    return 0;
}

unsigned _dos_findclose( struct find_t *buf )
{
    APIRET rc;

    if (HANDLE_OF(buf) != BAD_HANDLE)
    {
        rc = DosFindClose(HANDLE_OF(buf));

        if (rc != 0)
        {
            errno = ENOENT;
            return 1;
        }
    }

    return 0;
}

#endif

char *os_findfirst(struct _filefind *pff, const char *path, const char *mask)
{
    unsigned rc;
    char tmp[MYMAXPATH];

    strlcpy(tmp, path, sizeof tmp);
    os_append_slash(tmp);
    strlcat(tmp, mask, sizeof tmp);

    rc = _dos_findfirst(tmp, _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH, &pff->fileinfo);

    if (rc == 0)
    {
        return pff->fileinfo.name;
    }

    return NULL;
}

char *os_findnext(struct _filefind *pff)
{
    if (_dos_findnext(&pff->fileinfo) == 0)
    {
        return pff->fileinfo.name;
    }

    return NULL;
}

void os_findclose(struct _filefind *pff)
{
#ifdef __BORLANDC__
    /* Borland C++ for OS/2 doesn't have _dos_findclose */
    unused(pff);
#else
    _dos_findclose(&pff->fileinfo);
#endif
}

#elif defined(OS_DOS) && defined(__TURBOC__)

char *os_findfirst(struct _filefind *pff, const char *path, const char *mask)
{
    int rc;
    char tmp[MAXPATH];

    strlcpy(tmp, path, sizeof tmp);
    os_append_slash(tmp);
    strlcat(tmp, mask, sizeof tmp);

    rc = findfirst(tmp, &pff->fileinfo, FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH);

    if (rc == 0)
    {
        return pff->fileinfo.ff_name;
    }

    return NULL;
}

char *os_findnext(struct _filefind *pff)
{
    if (findnext(&pff->fileinfo) == 0)
    {
        return pff->fileinfo.ff_name;
    }

    return NULL;
}

void os_findclose(struct _filefind *pff)
{
    unused(pff);
}

#elif defined(_MSC_VER) && defined(OS_DOS)

char *os_findfirst(struct _filefind *pff, const char *path, const char *mask)
{
    unsigned rc;
    char tmp[MYMAXPATH];

    strlcpy(tmp, path, sizeof tmp);
    os_append_slash(tmp);
    strlcat(tmp, mask, sizeof tmp);

    rc = _dos_findfirst(tmp, _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH, &pff->fileinfo);

    if (rc == 0)
    {
        return pff->fileinfo.name;
    }

    return NULL;
}

char *os_findnext(struct _filefind *pff)
{
    if (_dos_findnext(&pff->fileinfo) == 0)
    {
        return pff->fileinfo.name;
    }

    return NULL;
}

void os_findclose(struct _filefind *pff)
{
    unused(pff);
}

#elif defined(__BORLANDC__) || defined(_MSC_VER) || defined(__DMC__) || defined(__LCC__) || defined(__POCC__) || defined(__MINGW32__)

char *os_findfirst(struct _filefind *pff, const char *path, const char *mask)
{
    char tmp[FILENAME_MAX];

    strlcpy(tmp, path, sizeof tmp);
    os_append_slash(tmp);
    strlcat(tmp, mask, sizeof tmp);
    pff->handle = _findfirst(tmp, &pff->fileinfo);

    if (pff->handle == -1)
    {
        return NULL;
    }

    return pff->fileinfo.name;
}

char *os_findnext(struct _filefind *pff)
{
    if (_findnext(pff->handle, &pff->fileinfo) == 0)
    {
        return pff->fileinfo.name;
    }

    return NULL;
}

void os_findclose(struct _filefind *pff)
{
    _findclose(pff->handle);
}

#endif

#if defined(OS_DOS)

/*
 *  os_fullpath()
 *
 *  Make an absolute path from given relative path.
 */

#if defined(__TURBOC__) || defined(__DJGPP__)

int os_fullpath(char *dst, const char *src, size_t bufsiz)
{
    int rc = -1;
    char tmp[MYMAXDIR];
    char curdir[MYMAXDIR];
    int curdrnum;
    int reqdrnum;

    curdrnum = getdisk();
    mklog(LOG_DEBUG, "os_fullpath(): Old drive number: %d", curdrnum);

    if (!src || !*src)
    {
        src = ".";
    }

    if (src[1] == ':')
    {
        /* drive letter specified */

        mklog(LOG_DEBUG, "os_fullpath(): You specified a drive letter");
        reqdrnum = toupper(src[0]) - 'A';

        if (reqdrnum != curdrnum)
        {
            /* requested drive is not current drive */
            mklog(LOG_DEBUG, "os_fullpath(): Switching to drive %d", reqdrnum);

            /* set current disk */
            setdisk(reqdrnum);

            if (getdisk() != reqdrnum)
            {
                mklog(LOG_DEBUG, "os_fullpath(): setdisk(%d) failed.", reqdrnum);
                return -1;
            }
        }

        /* Skip drive letter */
        src += 2;
    }

    mklog(LOG_DEBUG, "os_fullpath(): Searching for '%s' on current drive", src);

    if (getcwd(curdir, bufsiz) != NULL)
    {
        char *fname;
        char *dir = tmp;
        int chdir_rc;

        /*
         * Requested drive is now current drive, curdrnum is the original drive
         * reqdrnum is the requested drive curdir is the original directory on
         * the current drive src is the requested relative path without drive.
         */

        strcpy(dir, src);
        fname = strrchr(dir, '\\');
        if (!fname)
        {
            fname = strrchr(dir, '/');
        }

        if (!fname)
        {
            /* no backslash */

            mklog(LOG_DEBUG, "os_fullpath(): You don't have any backslash in the file name.");

            if (!(dir[0] == '.' && (dir[1] == '.' || dir[1] == '\0')))
            {
                mklog(LOG_DEBUG, "os_fullpath(): I assume this file is relative to cwd.");
                fname = dir;
                dir = ".";
            }
            else
            {
                mklog(LOG_DEBUG, "os_fullpath(): Looks like relative directory only");
                fname = "";
            }
        }
        else
        {
            *fname++ = '\0';
        }

        mklog(LOG_DEBUG, "os_fullpath(): Directory is now '%s', File name is now '%s'", dir, fname);

        /* fname = pure file name */
        /* dir = relative path, only directory */

        /* If dir is empty it means root directory */

        if (chdir((*dir) ? dir : "\\") == 0)
        {
            mklog(LOG_DEBUG, "os_fullpath(): chdir() succeeded. The directory exists.");

            if (getcwd(dst, bufsiz) == NULL)
            {
                mklog(LOG_DEBUG, "os_fullpath(): getcwd() failed.");
            }
            else
            {
                rc = 0;

                if (fname && *fname)
                {
                    if (strlen(dst) != 3)
                    {
                        /* Does not look like "C:\" */
                        strcat(dst, "\\");
                    }

                    strcat(dst, fname);
                }

                mklog(LOG_DEBUG, "os_fullpath(): Final full name is '%s'", fname);
            }
        }

        chdir_rc = chdir(curdir);

        if (chdir_rc != 0)
        {
            mklog(LOG_DEBUG, "os_fullpath(): chdir() to '%s' failed!", curdir);
        }
    }

    setdisk(curdrnum);

    if (getdisk() != curdrnum)
    {
        mklog(LOG_DEBUG, "os_fullpath(): setdisk(%d) failed.", curdrnum);
        return -1;
    }

    os_dirsep(dst);

    return rc;
}

#endif

#if defined (__WATCOMC__) || defined(_MSC_VER)

int os_fullpath(char *dst, const char *src, size_t bufsiz)
{
    int rc = -1;
    char tmp[MYMAXDIR];
    char curdir[MYMAXDIR];
    unsigned curdrnum;
    unsigned reqdrnum;
    unsigned total;
    unsigned temp;

    _dos_getdrive(&curdrnum);
    mklog(LOG_DEBUG, "os_fullpath(): Old drive number: %d", curdrnum);

    if (!src || !*src)
    {
        src = ".";
    }

    if (src[1] == ':')
    {
        /* drive letter specified */

        mklog(LOG_DEBUG, "os_fullpath(): You specified a drive letter");
        reqdrnum = toupper(src[0]) - 'A' + 1;

        if (reqdrnum != curdrnum)
        {
            /* requested drive is not current drive */
            mklog(LOG_DEBUG, "os_fullpath(): Switching to drive %d", reqdrnum);

            /* set current disk */
            _dos_setdrive(reqdrnum, &total);
            _dos_getdrive(&temp);

            if (temp != reqdrnum)
            {
                mklog(LOG_DEBUG, "os_fullpath(): _dos_setdrive(%d, &total) failed.", reqdrnum);
                return -1;
            }
        }

        /* Skip drive letter */
        src += 2;
    }

    mklog(LOG_DEBUG, "os_fullpath(): Searching for '%s' on current drive", src);

    if (getcwd(curdir, bufsiz) != NULL)
    {
        char *fname;
        char *dir = tmp;

        /*
         * Requested drive is now current drive, curdrnum is the original drive
         * reqdrnum is the requested drive curdir is the original directory on
         * the current drive src is the requested relative path without drive.
         */

        strcpy(dir, src);
        fname = strrchr(dir, '\\');
        if (!fname)
        {
            fname = strrchr(dir, '/');
        }

        if (!fname)
        {
            /* no backslash */

            mklog(LOG_DEBUG, "os_fullpath(): You don't have any backslash in the file name.");

            if (!(dir[0] == '.' && (dir[1] == '.' || dir[1] == '\0')))
            {
                mklog(LOG_DEBUG, "os_fullpath(): I assume this file is relative to cwd.");
                fname = dir;
                dir = ".";
            }
            else
            {
                mklog(LOG_DEBUG, "os_fullpath(): Looks like relative directory only");
                fname = "";
            }
        }
        else
        {
            *fname++ = '\0';
        }

        mklog(LOG_DEBUG, "os_fullpath(): Directory is now '%s', File name is now '%s'", dir, fname);

        /* fname = pure file name */
        /* dir = relative path, only directory */

        /* If dir is empty it means root directory */

        if (chdir((*dir) ? dir : "\\") == 0)
        {
            mklog(LOG_DEBUG, "os_fullpath(): chdir() succeeded. The directory exists.");

            if (getcwd(dst, bufsiz) != NULL)
            {
                rc = 0;

                if (fname && *fname)
                {
                    if (strlen(dst) != 3)
                    {
                        /* Does not look like "C:\" */
                        strcat(dst, "\\");
                    }

                    strcat(dst, fname);
                }

                mklog(LOG_DEBUG, "os_fullpath(): Final full name is '%s'", fname);
            }
        }

        if (chdir(curdir) != 0)
        {
            mklog(LOG_DEBUG, "os_fullpath(): chdir('%s') failed", curdir);
        }
    }

    _dos_setdrive(curdrnum, &total);
    _dos_getdrive(&temp);

    if (temp != curdrnum)
    {
        mklog(LOG_DEBUG, "os_fullpath(): _dos_setdrive(%d, &total) failed.", curdrnum);
    }

    os_dirsep(dst);

    return rc;
}
#endif

#elif defined(__EMX__)

/* EMX's _fullpath() is not like the others... */

int os_fullpath(char *dst, const char *src, size_t bufsiz)
{
    int result;
    result = _fullpath(dst, src, bufsiz);
    /* TODO: check correct return values of EMX's _fullpath() */
    os_dirsep(dst);
    return result;
}

#else

int os_fullpath(char *dst, const char *src, size_t bufsiz)
{
    char *path;

    path = _fullpath(dst, (char *) src, bufsiz);

    if (path == NULL)
    {
        mklog(LOG_DEBUG, "os_fullpath(): _fullpath() failed.");
        return -1;
    }

    os_dirsep(dst);

    return 0;
}

#endif

#endif

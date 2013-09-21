/* $Id: os.c,v 1.17 2013/09/21 11:20:02 ozzmosis Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "os.h"

#if defined(OS_UNIX)
#include <dirent.h>
#ifdef __linux__
/* _GNU_SOURCE needed for FNM_CASEFOLD */
#define _GNU_SOURCE
#endif
#include <fnmatch.h>
#endif

#include "fileutil.h"
#include "mklog.h"

char *os_file_getname(const char *path)
{
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
}

/*
 *  os_chdir()
 *
 *  Workaround for systems where chdir() will not accept a trailing \ character unless
 *  it's the root of the drive.
 */

#if defined(OS_DOS) || defined(OS_OS2) || defined(OS_WIN)

static int validdriveletter(int drive)
{
    char driveletters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return strchr(driveletters, toupper(drive)) != NULL;
}

int os_chdir(char *path)
{
    char *newpath, *p;
    int rc;

    mklog(LOG_DEBUG, "os_chdir path='%s'", path);

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

    if (validdriveletter(path[0]) && path[1] == ':' && path[2] == '\\' && path[3] == '\0')
    {
        /* "x:\" is OK too */

        return chdir(path);
    }

    /* copy path and modify it */

    newpath = malloc(strlen(path) + 1);

    if (newpath == NULL)
    {
        /* out of memory */
        return 1;
    }

    strcpy(newpath, path);

    /* strip trailing \ */
    os_remove_slash(newpath);

    mklog(LOG_DEBUG, "os_chdir newpath='%s'", newpath);
    rc = chdir(newpath);
    free(newpath);
    return rc;
}

#else

int os_chdir(char *path)
{
    return chdir(path);
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
    char *cmd;
    int rc;

    cmd = malloc(strlen(command) + 1 + strlen(cmdline) + 1);
    if (!cmd)
    {
        mklog(LOG_ERROR, "os_spawn(): out of memory for command line buffer");
        return -1;
    }

    sprintf(cmd, "%s %s", command, cmdline);
    mklog(LOG_DEBUG, "os_spawn: %s", cmd);
    rc = system(cmd);
    mklog(LOG_DEBUG, "os_spawn: rc=%d", rc);

    free(cmd);
    return rc;
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

    strcpy(tmp, src);
    os_remove_slash(tmp);
    mklog(LOG_DEBUG, __FILE__ ": os_fulldir(): after removing slash: '%s'", tmp);

    if (os_fullpath(dst, tmp, bufsiz) != 0)
    {
        mklog(LOG_DEBUG, __FILE__ ": os_fulldir(): os_fullpath failed!");
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

#ifdef OS_UNIX

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

#if defined(OS_UNIX)

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

#if defined(__unix__) || defined(__MSDOS__)
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

    myfnsplit(src, NULL, dir, name, ext);
    mklog(LOG_DEBUG, "os_fullpath(): dir=%s, name=%s, ext=%s", dir, name, ext);
    getcwd(olddir, sizeof olddir);
    mklog(LOG_DEBUG, "os_fullpath(): old directory=%s", olddir);

    if (dir[0] && chdir(dir) == -1)
    {
        mklog(LOG_ERROR, "os_fullpath(): change directory to '%s' failed!", dir);
        return -1;
    }

    if (getcwd(dir, MYMAXDIR) == NULL || strlen(dir) + strlen(name) + strlen(ext) > bufsize)
    {
        mklog(LOG_ERROR, "os_fullpath(): Directory name for '%s' too long!", src);
        chdir(olddir);
        return -1;
    }

    myfnmerge(dst, NULL, dir, name, ext);
    mklog(LOG_DEBUG, "os_fullpath(): complete filename: %s", dst);
    chdir(olddir);
    return 0;
}

#endif

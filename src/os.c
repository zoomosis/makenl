/* os.c -- Operating system dependant functions for makenl */

/* $Id: os.c,v 1.14 2013/09/21 09:50:50 ozzmosis Exp $ */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "fileutil.h"
#include "makenl.h"
#include "os.h"
#include "mklog.h"

#ifndef OSFUL
#error "No os_fullpath for this compiler/OS, giving up!"
#endif
#include OSFUL

#ifndef OSFND
#error "No os_findfirst/os_findnext/os_findclose for this compiler/OS, giving up!"
#endif
#include OSFND

#ifdef USE_OWN_FGETS
#include USE_OWN_FGETS
#endif

char *os_file_getname(const char *path)
{
    const char *p;

    p = path;
    while (*path != 0)
        switch (*path++)
        {
        case ':':
        case '/':
        case '\\':
            p = path;           /* Note that PATH has been incremented */
            break;
        }
    return (char *)p;
}

/*
 *  os_chdir()
 *
 *  Workaround for systems where chdir() will not accept a trailing \ character unless
 *  it's the root of the drive.
 */

#if defined(__MSDOS__) || defined(__OS2__) || defined(WIN32)

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
            *(++p) = CHAR_DIRSEPARATOR;
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

#if defined(__MSDOS__) || defined(__OS2__) || defined(WIN32)
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
 * os_deslashify()
 *
 * Converts / slashes to \ backslashes for pathnames, for DOS, OS/2 & Windows systems.
 */

char *os_deslashify(char *path)
{
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

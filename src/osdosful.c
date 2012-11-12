/* $Id: osdosful.c,v 1.4 2012/11/12 17:30:01 ozzmosis Exp $ */

#define HAVE_OS_FULLPATH

#include <ctype.h>

#define FUNCNAME "[dos] os_fullpath"
/* make an absolute path from given relative path */
int os_fullpath(char *dst, const char *src, size_t bufsiz)
{
    int rc = -1;
    char tmp[MYMAXDIR];
    char curdir[MYMAXDIR];
    int curdrnum;
    int reqdrnum;

    curdrnum = getdisk();
    mklog(LOG_DEBUG, "Old drive number: %d", curdrnum);
    if (!src || !*src)
        src = ".";

    if (src[1] == ':')          /* drive letter specified */
    {
        mklog(LOG_DEBUG, "You specified a drive letter");
        reqdrnum = toupper(src[0]) - 'A';
        if (reqdrnum != curdrnum) /* requested drive is not current drive */
        {
            mklog(LOG_DEBUG, "Switching to drive %d", reqdrnum);
            setdisk(reqdrnum);  /* set current disk */
            if (getdisk() != reqdrnum) /* Specified drive does not exist */
            {
                mklog(LOG_DEBUG, "This drive does not exist");
                return -1;
            }
        }
        src += 2;               /* Skip drive letter */
    }

    mklog(LOG_DEBUG, "Searching for '%s' on current drive", src);
    if (getcwd(curdir, bufsiz) != NULL)
    {
        char *fname;
        char *dir = tmp;

        /* requested drive is now current drive, curdrnum is the original
           drive reqdrnum is the requested drive curdir is the original
           directory on the current drive src is the requested relative
           path without drive */
        strcpy(dir, src);
        fname = strrchr(dir, '\\');
        if (!fname)
            fname = strrchr(dir, '/');
        if (!fname)             /* no backslash */
        {
            mklog(LOG_DEBUG, "You don't have any backslash in the file name.");
            if (!(dir[0] == '.' && (dir[1] == '.' || dir[1] == '\0')))
            {
                mklog(LOG_DEBUG, "I assume this file is relative to cwd.");
                fname = dir;
                dir = ".";
            }
            else
            {
                mklog(LOG_DEBUG, "Looks like relative directory only");
                fname = "";
            }
        }
        else
            *fname++ = '\0';

        mklog(LOG_DEBUG, "Directory is now %s, File name is now %s", dir, fname);
        /* fname = pure file name */
        /* dir = relative path, only directory */

        /* If dir is empty it means root directory */
        if (chdir((*dir) ? dir : "\\") == 0)
        {
            mklog(LOG_DEBUG, "chdir() suceeded. The directory exists.");
            if (getcwd(dst, bufsiz) != NULL)
            {
                rc = 0;
                if (fname && *fname)
                {
                    if (strlen(dst) != 3) /* Does not look like "C:\" */
                        strcat(dst, "\\");
                    strcat(dst, fname);
                }
                mklog(LOG_DEBUG, "final full name is %s", fname);
            }
        }
        chdir(curdir);
    }
    setdisk(curdrnum);
    os_filecanonify(dst);
    return rc;
}

#undef FUNCNAME

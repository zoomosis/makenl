/* $Id: osdosful.c,v 1.3 2012/10/14 14:49:17 ozzmosis Exp $ */

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
    Debug1("Old drive number: %d\n", curdrnum);
    mklog(LOG_DEBUG, "Old drive number: %d", curdrnum);
    if (!src || !*src)
        src = ".";

    if (src[1] == ':')          /* drive letter specified */
    {
        Debug("You specified a drive letter\n");
        mklog(LOG_DEBUG, "You specified a drive letter");
        reqdrnum = toupper(src[0]) - 'A';
        if (reqdrnum != curdrnum) /* requested drive is not current drive */
        {
            Debug1("Switching to drive %d\n", reqdrnum);
            mklog(LOG_DEBUG, "Switching to drive %d", reqdrnum);
            setdisk(reqdrnum);  /* set current disk */
            if (getdisk() != reqdrnum) /* Specified drive does not exist */
            {
                Debug("This drive does not exist\n");
                mklog(LOG_DEBUG, "This drive does not exist");
                return -1;
            }
        }
        src += 2;               /* Skip drive letter */
    }

    Debug1("Searching for '%s' on current drive\n", src);
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
            Debug("You don't have any backslash in the file name.\n");
            mklog(LOG_DEBUG, "You don't have any backslash in the file name.");
            if (!(dir[0] == '.' && (dir[1] == '.' || dir[1] == '\0')))
            {
                Debug("I assume this file is relative to cwd.\n");
                mklog(LOG_DEBUG, "I assume this file is relative to cwd.");
                fname = dir;
                dir = ".";
            }
            else
            {
                Debug("Looks like relative directory only");
                mklog(LOG_DEBUG, "Looks like relative directory only");
                fname = "";
            }
        }
        else
            *fname++ = '\0';

        Debug1("Directory is now %s\n", dir);
        Debug1("File name is now %s\n", fname);
        mklog(LOG_DEBUG, "Directory is now %s, File name is now %s", dir, fname);
        /* fname = pure file name */
        /* dir = relative path, only directory */

        /* If dir is empty it means root directory */
        if (chdir((*dir) ? dir : "\\") == 0)
        {
            Debug("chdir() suceeded. The directory exists.\n");
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
                Debug1("final full name is %s\n", fname);
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

/* $Id: osgenrms.c,v 1.2 2004-07-11 09:29:14 ozzmosis Exp $ */

/* remove slash/backslash from last part of path name, if present */
char *os_remove_slash(char *path1)
{
    char *p;
    char *path = path1;

    if (!path)
        return NULL;

    if (*path && path[1] == ':')
        path += 2;              /* skip drive letter */

    if ((p = strchr(path, '\0')) > path)
    {
        p--;
        if (p > path && (*p == '\\' || *p == '/'))
            *p = '\0';
    }
    return path1;
}

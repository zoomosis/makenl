/* $Id: osgenaps.c,v 1.2 2004/07/11 09:29:14 ozzmosis Exp $ */

/* normalize path                                      */
/* It is assumed that path has one more char allocated */
/* then strlen+1.                                      */
char *os_append_slash(char *path)
{
    char *p;

    if (!path)
        return NULL;

    if ((p = strchr(path, '\0')) > path)
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

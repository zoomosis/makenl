/* $Id: osgenaps.c,v 1.1 2009/01/08 20:07:47 mbroek Exp $ */

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

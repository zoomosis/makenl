/* $Id: osgenchd.c,v 1.1 2013/09/20 21:07:05 ajleary Exp $ */

/* Generic default os_chdir() - just calls chdir() from C library. */

int os_chdir(char *path)
{
    return chdir(path);
}

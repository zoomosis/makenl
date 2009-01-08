/* $Id: osgenfld.c,v 1.1 2009/01/08 20:07:47 mbroek Exp $ */

#include <sys/types.h>
#include <sys/stat.h>

/* make an absolute path from given relative path */
/* src is supposed to specify an existing directory name    */
#define FUNCNAME "[generic] os_fulldir"
int os_fulldir(char *dst, const char *src, size_t bufsiz)
{
    char tmp[MYMAXPATH];
    struct stat st;

    Debug1("called with src='%s'\n", src);
    mklog(4, "[generic] os_fulldir: called with src='%s'", src);
    strcpy(tmp, src);
    os_remove_slash(tmp);
    Debug1("after removing slash: '%s'\n", tmp);
    mklog(4, "[generic] os_fulldir: after removing slash: '%s'", tmp);
    if (os_fullpath(dst, tmp, bufsiz) != 0)
    {
        Debug("os_fullpath failed!\n");
	mklog(3, "[generic] os_fulldir: os_fullpath failed!");
        return -1;
    }
    Debug1("doing stat on %s\n", dst);
    mklog(3, "[generic] os_fulldir: absolute path %s", dst);
    if (stat(dst, &st) != 0)
    {
        Debug("os_fulldir: stat failed!\n");
	mklog(3, "[generic] os_fulldir: stat failed!");
        return -1;
    }
    Debug1("st_mode is now %o\n", st.st_mode);
    mklog(4, "[generic] os_fulldir: st_mode is now %o", st.st_mode);
    return ((st.st_mode & S_IFMT) == S_IFDIR) ? 0 : -1;
}

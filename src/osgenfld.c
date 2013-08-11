/* $Id: osgenfld.c,v 1.4 2013/08/11 23:05:43 ozzmosis Exp $ */

#include <sys/types.h>
#include <sys/stat.h>

/* make an absolute path from given relative path */
/* src is supposed to specify an existing directory name    */
#define FUNCNAME "[generic] os_fulldir"
int os_fulldir(char *dst, const char *src, size_t bufsiz)
{
    char tmp[MYMAXPATH];
    struct stat st;

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
    if (stat(dst, &st) != 0)
    {
        mklog(LOG_DEBUG, __FILE__ ": os_fulldir(): stat failed!");
        return -1;
    }
    mklog(LOG_DEBUG, __FILE__ ": os_fulldir(): st_mode is now %o", st.st_mode);
#ifdef __TURBOC__
    return S_ISDIR(st.st_mode) ? 0 : -1;
#else
    return ((st.st_mode & S_IFMT) == S_IFDIR) ? 0 : -1;
#endif
}

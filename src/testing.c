#include <stdlib.h>
#include <string.h>

#include "makenl.h"
#include "mklog.h"
#include "os.h"

static void test_exit(int rc)
{
    mklog(LOG_INFO, "test_exit(%d)", rc);
    exit(rc);
}

static void test_findfirst(void)
{
    struct _filefind pff;
    char *p;
    mklog(LOG_INFO, "test_findfirst() begin");
    p = os_findfirst(&pff, DIRSEP, "*");
    while (p != NULL)
    {
        mklog(LOG_INFO, "  p == '%s'", p);
        p = os_findnext(&pff);
    }
    os_findclose(&pff);
    mklog(LOG_INFO, "test_findfirst() end");
}

static void test_findfile(void)
{
    struct _filefind pff;
    char *p;
    mklog(LOG_INFO, "test_findfile() begin");
    p = os_findfile(&pff, ".", "m*");
    mklog(LOG_INFO, "  os_findfile(): rc == '%s'", p);
    mklog(LOG_INFO, "test_findfile() end");
}

static void test_file_getname(void)
{
    char *p;
    mklog(LOG_INFO, "test_file_getname() begin");
    p = os_file_getname("/makenl");
    mklog(LOG_INFO, "  os_file_getname(): rc == '%s'", p);
    mklog(LOG_INFO, "test_file_getname() end");
}

static void test_fullpath(void)
{
    int rc;
    char buf[MYMAXDIR];
    mklog(LOG_INFO, "test_fullpath() begin");
    rc = os_fullpath(buf, "./makenl", sizeof buf);
    mklog(LOG_INFO, "  os_fullpath(): rc == %d, buf == '%s'", rc, buf);
    mklog(LOG_INFO, "test_fullpath() end");
}

static void test_fulldir(void)
{
    int rc;
    char buf[MYMAXDIR];
    mklog(LOG_INFO, "test_fulldir() begin");
    rc = os_fulldir(buf, "./makenl", sizeof buf);
    mklog(LOG_INFO, "  os_fulldir(): rc == %d, buf == '%s'", rc, buf);
    mklog(LOG_INFO, "test_fulldir() end");
}

static void test_slash(void)
{
    char *p, buf[MYMAXDIR];
    mklog(LOG_INFO, "test_slash() begin");

    strcpy(buf, "makenl");
    mklog(LOG_INFO, "  buf == '%s'", buf);

    p = os_append_slash(buf);
    mklog(LOG_INFO, "  os_append_slash(buf): rc == '%s'", p);

    p = os_remove_slash(p);
    mklog(LOG_INFO, "  os_remove_slash(p): rc == '%s'", p);

    strcpy(buf, "makenl/");
    mklog(LOG_INFO, "  buf == '%s'", buf);

    /*
     *  os_dirsep() converts / to \ in pathnames on DOS/OS2/Win systems,
     *  otherwise this should be a no-op
     */

    p = os_dirsep(buf);
    mklog(LOG_INFO, "  os_dirsep(buf): rc == '%s'", p);

    mklog(LOG_INFO, "test_slash() end");
}

void testing(void)
{
    test_findfirst();
    test_findfile();
    test_file_getname();
    test_fullpath();
    test_fulldir();
    test_slash();
    test_exit(0);
}

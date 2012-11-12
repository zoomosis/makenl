/* $Id: osemxexc.c,v 1.5 2012/11/12 18:27:13 ozzmosis Exp $ */

#include <process.h>
#include "mklog.h"

/* spawn sub-process (currently used for compress/decompress tool) */
int os_spawn(const char *command, const char *cmdline)
{
    char execfn[_MAX_PATH];
    char tmpfn[_MAX_PATH];
    char *pext;
    char *cmd;
    int rc;

    /* search for command */
    strcpy(tmpfn, command);
    pext = strchr(tmpfn, '\0');

    mklog(LOG_DEBUG, "os_spawn: trying `%s'", tmpfn);
    rc = _path(execfn, tmpfn);
    if (rc != 0)
    {
        strcpy(pext, ".EXE");
        mklog(LOG_DEBUG, "os_spawn: trying `%s'", tmpfn);
        rc = _path(execfn, tmpfn);
    }
    else if (rc != 0 && _osmode == OS2_MODE)
    {
        strcpy(pext, ".CMD");
        mklog(LOG_DEBUG, "os_spawn: trying `%s'", tmpfn);
        rc = _path(execfn, tmpfn);
    }
    else if (rc != 0 && _osmode != OS2_MODE)
    {
        strcpy(pext, ".COM");
        mklog(LOG_DEBUG, "os_spawn: trying `%s'", tmpfn);
        rc = _path(execfn, tmpfn);
    }
    else if (rc != 0 && _osmode != OS2_MODE)
    {
        strcpy(pext, ".BAT");
        mklog(LOG_DEBUG, "os_spawn: trying `%s'", tmpfn);
        rc = _path(execfn, tmpfn);
    }
    else if (rc != 0)
    {
        mklog(LOG_ERROR, "os_spawn(): program not found");
        return -1;
    }

    cmd = malloc(strlen(command) + 1 + strlen(cmdline) + 1);
    if (!cmd)
        return -1;

    sprintf(cmd, "%s %s", command, cmdline);
    mklog(LOG_DEBUG, "found: executing `%s'", cmd);
    rc = system(cmd);
    mklog(LOG_DEBUG, "os_spawn rc=%d", rc);

    free(cmd);
    return rc;
}

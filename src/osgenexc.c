/* $Id: osgenexc.c,v 1.4 2012/10/14 14:49:17 ozzmosis Exp $ */

#include "mklog.h"

int os_spawn(const char *command, const char *cmdline)
{
    char *cmd;
    int rc;

    cmd = malloc(strlen(command) + 1 + strlen(cmdline) + 1);
    if (!cmd)
    {
        mklog(LOG_ERROR, "os_spawn(): out of memory for command line buffer");
        return -1;
    }

    sprintf(cmd, "%s %s", command, cmdline);
    mklog(LOG_DEBUG, "os_spawn: %s", cmd);
    rc = system(cmd);
    mklog(LOG_DEBUG, "os_spawn: rc=%d", rc);

    free(cmd);
    return rc;
}

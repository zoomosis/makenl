/* $Id: osgenexc.c,v 1.2 2012/10/13 00:17:24 ozzmosis Exp $ */

#include "mklog.h"

int os_spawn(const char *command, const char *cmdline)
{
    char *cmd;
    int rc;

    cmd = malloc(strlen(command) + 1 + strlen(cmdline) + 1);
    if (!cmd)
    {
	mklog(0, "os_spawn: out of memory for command line buffer");
        return -1;
    }

    sprintf(cmd, "%s %s", command, cmdline);
    mklog(3, "os_spawn: %s", cmd);
    rc = system(cmd);
    mklog(3, "os_spawn: rc=%d", rc);

    free(cmd);
    return rc;
}

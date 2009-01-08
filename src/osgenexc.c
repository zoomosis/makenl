/* $Id: osgenexc.c,v 1.1.1.1 2009/01/08 20:07:47 mbroek Exp $ */

int os_spawn(const char *command, const char *cmdline)
{
    char *cmd;
    int rc;

    cmd = malloc(strlen(command) + 1 + strlen(cmdline) + 1);
    if (!cmd)
    {
        fprintf(stderr, "Out of memory for command line buffer!\n");
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

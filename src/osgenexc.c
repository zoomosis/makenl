/* $Id: osgenexc.c,v 1.3 2004/09/03 21:46:23 mbroek Exp $ */

int os_spawn(const char *command, const char *cmdline)
{
    char *cmd;
    int rc;

    cmd = malloc(strlen(command) + 1 + strlen(cmdline) + 1);
    if (!cmd)
    {
        fprintf(stderr, "Out of memory for command line buffer!\n");
        return -1;
    }

    sprintf(cmd, "%s %s", command, cmdline);
    mklog(2, "os_spawn: %s", cmd);
    rc = system(cmd);
    mklog(2, "os_spawn: rc=%d", rc);

    free(cmd);
    return rc;
}

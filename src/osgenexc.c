/* $Id: osgenexc.c,v 1.2 2004/07/11 09:29:14 ozzmosis Exp $ */

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
    rc = system(cmd);

    free(cmd);
    return rc;
}

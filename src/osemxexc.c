/* $Id: osemxexc.c,v 1.4 2004/09/05 10:43:57 mbroek Exp $ */

#include <process.h>

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

    Debug1("os_spawn: trying `%s'\n", tmpfn);
    mklog(4, "os_spawn: trying `%s'", tmpfn);
    rc = _path(execfn, tmpfn);
    if (rc != 0)
    {
        strcpy(pext, ".EXE");
        Debug1("os_spawn: trying `%s'\n", tmpfn);
	mklog(4, "os_spawn: trying `%s'", tmpfn);
        rc = _path(execfn, tmpfn);
    }
    else if (rc != 0 && _osmode == OS2_MODE)
    {
        strcpy(pext, ".CMD");
        Debug1("os_spawn: trying `%s'\n", tmpfn);
	mklog(4, "os_spawn: trying `%s'", tmpfn);
        rc = _path(execfn, tmpfn);
    }
    else if (rc != 0 && _osmode != OS2_MODE)
    {
        strcpy(pext, ".COM");
        Debug1("os_spawn: trying `%s'\n", tmpfn);
	mklog(4, "os_spawn: trying `%s'", tmpfn);
        rc = _path(execfn, tmpfn);
    }
    else if (rc != 0 && _osmode != OS2_MODE)
    {
        strcpy(pext, ".BAT");
        Debug1("os_spawn: trying `%s'\n", tmpfn);
	mklog(4, "os_spawn: trying `%s'", tmpfn);
        rc = _path(execfn, tmpfn);
    }
    else if (rc != 0)
    {
        printf("os_spawn: program not found\n");
	mklog(0, "os_spawn: program not found");
        return -1;
    }

    cmd = malloc(strlen(command) + 1 + strlen(cmdline) + 1);
    if (!cmd)
        return -1;

    sprintf(cmd, "%s %s", command, cmdline);
    Debug1("found: executing `%s'\n", cmd);
    mklog(3, "found: executing `%s'", cmd);
    rc = system(cmd);
    Debug1("os_spawn rc=%d\n", rc);
    mklog(3, "os_spawn rc=%d", rc);

    free(cmd);
    return rc;
}

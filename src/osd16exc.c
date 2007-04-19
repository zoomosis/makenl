/* $Id: osd16exc.c,v 1.3 2004-07-16 09:00:46 ozzmosis Exp $ */

#include "exec.h"

int os_spawn(const char *command, const char *cmdline)
{
    return do_exec(command, cmdline, USE_EMS | USE_XMS | USE_FILE,
      0xffff, environ);
}

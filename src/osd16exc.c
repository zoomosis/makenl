/* $Id: osd16exc.c,v 1.1.1.1 2009/01/08 20:07:47 mbroek Exp $ */

#include "exec.h"

int os_spawn(const char *command, const char *cmdline)
{
    return do_exec(command, cmdline, USE_EMS | USE_XMS | USE_FILE,
      0xffff, environ);
}

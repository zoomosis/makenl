#include "exec.h"

int os_spawn(const char *command, const char *cmdline)
{
    return do_exec(command, cmdline, USE_EMS | USE_XMS | USE_FILE,
      0xffff, environ);
}

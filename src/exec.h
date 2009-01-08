/* $Id: exec.h,v 1.1 2009/01/08 20:07:46 mbroek Exp $ */

#ifndef _EXEC_H
#define _EXEC_H

/* EXEC.H: EXEC function with memory swap - Main function header file.

   Public domain software by

   Thomas Wagner
   Ferrari electronic GmbH
   Beusselstrasse 27
   D-1000 Berlin 21
   Germany
 */

extern int do_exec(const char *xfn, const char *pars, int spawn,
                   unsigned needed, char **envp);

/*
   The EXEC function.

   Parameters:

   xfn      is a string containing the name of the file
   to be executed.

   pars     The program parameters.

   spawn    If 0, the function will terminate after the
   EXECed program returns, the function will not return.

   NOTE: If the program file is not found, the function
   will always return with the appropriate error
   code, even if 'spawn' is 0.

   If non-0, the function will return after executing the
   program. If necessary (see the "needed" parameter),
   memory will be swapped out before executing the program.
   For swapping, spawn must contain a combination of the
   following flags:

   USE_EMS  (0x01)  - allow EMS swap
   USE_XMS  (0x02)  - allow XMS swap
   USE_FILE (0x04)  - allow File swap

   The order of trying the different swap methods can be
   controlled with one of the flags

   EMS_FIRST (0x00) - EMS, XMS, File (default)
   XMS_FIRST (0x10) - XMS, EMS, File

   If swapping is to File, the attribute of the swap file
   can be set to "hidden", so users are not irritated by
   strange files appearing out of nowhere with the flag

   HIDE_FILE (0x40)    - create swap file as hidden

   and the behaviour on Network drives can be changed with

   NO_PREALLOC (0x100) - don't preallocate
   CHECK_NET (0x200)   - don't preallocate if file on net.

   This checking for Network is mainly to compensate for
   a strange slowdown on Novell networks when preallocating
   a file. You can either set NO_PREALLOC to avoid allocation
   in any case, or let the prep_swap routine decide whether
   to do preallocation or not depending on the file being
   on a network drive (this will only work with DOS 3.1 or
   later).

   needed   The memory needed for the program in paragraphs (16 Bytes).
   If not enough memory is free, the program will
   be swapped out.
   Use 0 to never swap, 0xffff to always swap.
   If 'spawn' is 0, this parameter is irrelevant.

   envp     The environment to be passed to the spawned
   program. If this parameter is NULL, a copy
   of the parent's environment is used (i.e.
   'putenv' calls have no effect). If non-NULL,
   envp must point to an array of pointers to
   strings, terminated by a NULL pointer (the
   standard variable 'environ' may be used).

   Return value:

   0x0000..00FF: The EXECed Program's return code
   0x0101:       Error preparing for swap: no space for swapping
   0x0102:       Error preparing for swap: program too low in memory
   0x0200:       Program file not found
   0x03xx:       DOS-error-code xx calling EXEC
   0x0400:       Error allocating environment buffer
   0x0500:       Swapping requested, but prep_swap has not
   been called or returned an error.
   0x0501:       MCBs don't match expected setup
   0x0502:       Error while swapping out
 */

/* Return codes (only upper byte significant) */

# define RC_PREPERR   0x0100
# define RC_NOFILE    0x0200
# define RC_EXECERR   0x0300
# define RC_ENVERR    0x0400
# define RC_SWAPERR   0x0500

/* Swap method and option flags */

# define USE_EMS      0x01
# define USE_XMS      0x02
# define USE_FILE     0x04
# define EMS_FIRST    0x00
# define XMS_FIRST    0x10
# define HIDE_FILE    0x40
# define NO_PREALLOC  0x100
# define CHECK_NET    0x200

# define USE_ALL      (USE_EMS | USE_XMS | USE_FILE)

#endif

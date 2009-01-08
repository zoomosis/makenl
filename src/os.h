/* $Id: os.h,v 1.1 2009/01/08 20:07:47 mbroek Exp $ */

#ifndef _OS_H
#define _OS_H

/* Operating system and compiler dependant functions */
/* Definitions for:                                               */
/* __GNUC__     GNU C Compiler                                    */
/* __EMX__      emx for DOS/OS2                                   */
/* __WATCOMC__  Watcom C Compiler 10.0b under DOS, OS/2 and Win32 */
/* __TURBOC__   Turbo C 2.00 and above, Borland C, under DOS      */

/* Operating systems/memory model:
   __FLAT__   32-bit flat memory model
   __SMALL__  DOS small memory model
   __MSDOS__  DOS 16 or 32-bit
   __DOS16__  DOS 16-bit
   __DOS32__  DOS 32-bit (currently: dos4gw for Watcom)
   __EMX__    emx+gcc (32-bit dual executable DOS/OS2)
   __OS2__    OS/2 2.0 and above
   __W32__    Win32 (Win9x/NT)
 */

#ifdef __GNUC__
#define __FLAT__                /* flat memory model, min. 32-bit */
#endif

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

/* Defaults for overwriteable functions */
#define OSAPS "osgenaps.c"
#define OSRMS "osgenrms.c"
#define OSCAN "osgencan.c"
#define OSDSL "osgendsl.c"
#define OSEXC "osgenexc.c"
#define OSFF "osgenff.c"
#define OSFLD "osgenfld.c"
#define OSGTN "osgengtn.c"


/* GNU C/EMX for DOS & OS/2 */
#if defined(__GNUC__) && defined(__EMX__)
#include "osgnuemx.h"
#define	MAKENL_CC   "EMX"
					  
/* GNU C/Linux */
#elif defined(__GNUC__) && defined(__linux__)
#include "osgnulnx.h"
#define	MAKENL_CC   "GCC/Linux"
					  
/* GNU C/FreeBSD */
#elif defined(__GNUC__) && defined(__FreeBSD__)
/* we'll use the Linux ones */
#include "osgnulnx.h"
#define	MAKENL_CC    "GCC/FreeBSD"

/* GNU C/Apple OS X (Darwin) */
#elif defined(__GNUC__) && defined(__APPLE__)
/* we'll use the Linux ones */
#include "osgnulnx.h"
#define MAKENL_CC    "GCC/OS X"

/* GNU C/DJGPP 2.0 for DOS */
#elif defined(__GNUC__) && defined(__MSDOS__)
#include "osgnudjg.h"
#define	MAKENL_CC   "GCC C/DJGPP"

/* GNU C/Dev-C++ for Windows */
#elif defined(__GNUC__) && defined(__MINGW32__)
/* use Microsoft Visual C++ headers */
#include "osmscwin.h"
#define	MAKENL_CC   "GNU C/Dev-C++"

/* Borland Turbo C++ 1.0, Borland C++ 3.1 & 4.0, etc. for DOS */
#elif defined(__TURBOC__) && defined(__MSDOS__)
#include "ostbcdos.h"
#define	MAKENL_CC   "Borland Turbo C++"

/* Watcom C/C++ for DOS, OS/2 & Windows */
#elif defined(__WATCOMC__)
#include "oswatxxx.h"
#define	MAKENL_CC   "Watcom C/C++"

/* Microsoft Visual C++ for Windows */
#elif defined(_MSC_VER) && defined(WIN32)
#include "osmscwin.h"
#define	MAKENL_CC   "Microsoft Visual C++"

#elif defined(__BORLANDC__) && defined(__WIN32__)
#include "osborwin.h"
#define MAKENL_CC   "Borland C++"

#else
#error "It seems to me that you are using an unknown compiler!"
#endif

/* Platforms */
#if defined (__DOS16__)
#define	MAKENL_OS   "DOS 16"
#elif defined (__DOS4G__)
#define	MAKENL_OS   "DOS 32"
#elif defined (__OS2__)
#define	MAKENL_OS   "OS/2 16"
#elif defined (__OS2V2__)
#define MAKENL_OS   "OS/2 32"
#elif defined (__W32__)
#define	MAKENL_OS   "Windows/32"
#elif defined (__MSDOS__)
#define	MAKENL_OS   "MS-DOS"
#elif defined (__linux__)
#define	MAKENL_OS   "GNU/Linux"
#elif defined (__FreeBSD__)
#define MAKENL_OS   "FreeBSD"
#elif defined (__APPLE__)
#define MAKENL_OS   "OS X (Darwin)"
#elif defined (__NetBSD__)
#define MAKENL_OS   "NetBSD"
#elif defined (__OpenBSD__)
#define MAKENL_OS   "OpenBSD"
#else
#error "Unknown OS!"
#endif

					  
#ifndef STR_DIRSEPARATOR
#error "No one defined STR_DIRSEPARATOR!"
#else
#define CHAR_DIRSEPARATOR STR_DIRSEPARATOR[0]
#endif

/* ------------------------------------------------------------------------- */
/* os-independant global function declarations                               */

char *os_findfirst(struct _filefind *pff, const char *path,
                   const char *mask);
char *os_findnext(struct _filefind *pff);
void os_findclose(struct _filefind *pff);
char *os_findfile(struct _filefind *pff, const char *path,
                  const char *mask);

char *os_file_getname(const char *path);
int os_fullpath(char *dst, const char *src, size_t bufsiz);
int os_fulldir(char *dst, const char *src, size_t bufsiz);
char *os_getcwd(char *buffer, int size);
char *os_append_slash(char *path);
char *os_remove_slash(char *path);
char *os_deslashify(char *name);
char *os_filecanonify(char *s);

int os_spawn(const char *command, const char *cmdline);

#ifndef USE_OWN_GETCWD
#define os_getcwd getcwd
#endif


#ifdef USE_OWN_FGETS
#include <stdio.h>
char *os_fgets(char *buf, size_t len, FILE * f);
#define fgets os_fgets

#endif

#ifndef HAVE_STRUPR
char *strupr(char *string);
#endif


/* some sanity checks for compiler defines */

/* The GNUC-Check was thrown out. There can be GNU C with MSDOS, OS/2 or
   WIN32... */

#if defined(__FLAT__) && defined(__SMALL__)
#error "Both __FLAT__ and __SMALL__ defined!"
#endif

#if defined(__DOS16__) && defined(__FLAT__)
#error "Both __DOS16__ and __FLAT defined!"
#endif

#if defined(__DOS16__) && defined(__DOS32__)
#error "Both __DOS16__ and __DOS32__ defined!"
#endif

#if defined(__DOS16__) && defined(__DOS4G__)
#error "Both __DOS16__ and __DOS4G__ defined!"
#endif

#if defined(__MSDOS__) + defined(__OS2__) + defined(__OS2V2__) + defined(__W32__) > 1
#error "Ambigous OS specification"
#endif

#endif

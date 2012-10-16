/* $Id: os.h,v 1.4 2012/10/16 20:21:04 ozzmosis Exp $ */

#ifndef _OS_H
#define _OS_H

/* Defaults for overwriteable functions */
#define OSAPS "osgenaps.c"
#define OSRMS "osgenrms.c"
#define OSCAN "osgencan.c"
#define OSDSL "osgendsl.c"
#define OSEXC "osgenexc.c"
#define OSFF "osgenff.c"
#define OSFLD "osgenfld.c"
#define OSGTN "osgengtn.c"

#if defined(__clang__)
#define __FLAT__
#define MAKENL_CC "Clang"
#elif defined(__GNUC__)
#define __FLAT__  /* flat memory model */
#define MAKENL_CC "GNU C"
#elif defined(__TURBOC__) && defined(__MSDOS__)
#include "ostbcdos.h"
#define MAKENL_CC "Turbo C"
#elif defined(__WATCOMC__)
#include "oswatxxx.h"
#define MAKENL_CC "Watcom C"
#elif defined(_MSC_VER) && defined(WIN32)
#include "osmscwin.h"
#define MAKENL_CC "MSVC"
#elif defined(__BORLANDC__) && defined(__WIN32__)
#include "osborwin.h"
#define MAKENL_CC "Borland C"
#endif

#if defined(__GNUC__)
#if defined(__EMX__)
#include "osgnuemx.h"
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
#include "osgnulnx.h"
#elif defined(__DJGPP__)
#include "osgnudjg.h"
#elif defined(__MINGW32__)
/* use Microsoft Visual C++ headers */
#include "osmscwin.h"
#endif
#endif

#ifndef MAKENL_CC
#error "Unknown compiler detected. Cannot continue."
#endif

#if defined (__DOS16__)
#define MAKENL_OS "DOS16"
#elif defined (__DOS4G__)
#define MAKENL_OS "DOS32"
#elif defined (__EMX__)
#define MAKENL_OS "EMX"
#elif defined (__OS2__)
#define MAKENL_OS "OS/2 16-bit"
#elif defined (__OS2V2__)
#define MAKENL_OS "OS/2 32-bit"
#elif defined (__W32__)
#define MAKENL_OS "Win32"
#elif defined (__MSDOS__)
#define MAKENL_OS "MS-DOS"
#elif defined (__linux__)
#define MAKENL_OS "Linux"
#elif defined (__FreeBSD__)
#define MAKENL_OS "FreeBSD"
#elif defined (__APPLE__)
#define MAKENL_OS "Darwin"
#elif defined (__NetBSD__)
#define MAKENL_OS "NetBSD"
#elif defined (__OpenBSD__)
#define MAKENL_OS "OpenBSD"
#else
#error "Unknown build target detected. Cannot continue."
#endif


#ifndef STR_DIRSEPARATOR
#error "No one defined STR_DIRSEPARATOR!"
#else
#define CHAR_DIRSEPARATOR STR_DIRSEPARATOR[0]
#endif

/* os-independant global function declarations */

char *os_findfirst(struct _filefind *pff, const char *path, const char *mask);
char *os_findnext(struct _filefind *pff);
void os_findclose(struct _filefind *pff);
char *os_findfile(struct _filefind *pff, const char *path, const char *mask);

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

/* The GNUC-Check was thrown out. There can be GNU C with MSDOS, OS/2 or WIN32... */

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
#error "Ambiguous OS specification"
#endif

#endif

/* $Id: os.h,v 1.44 2013/09/21 14:31:45 ozzmosis Exp $ */

#ifndef __OS_H__
#define __OS_H__

#include <stdio.h>
#include <stdlib.h>

/*
 *  OS-specific #defines
 *
 *  OS_DOS   MS-DOS target
 *  OS_OS2   OS/2 target
 *  OS_WIN   Windows target
 *  OS_UNIX  UNIX target; includes FreeBSD, Linux, Apple OS X (Darwin), etc.
 *
 *
 *  Memory model defines
 *
 *  MEM_SEG  Segmented memory model (not flat)
 *
 *
 *  Compiler & OS #define strings (informational only)
 *
 *  CC_NAME  Compiler name
 *  OS_NAME  Operating system name
 */

#if defined(__MSDOS__) || defined(__DOS__)
#define OS_DOS 1
#elif defined(__OS2__) || defined(_OS2)
#define OS_OS2 1
#elif defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#define OS_WIN 1
#else
#define OS_UNIX 1
#endif

#if defined(OS_DOS) || defined(OS_OS2)
#if !defined(__386__) && !defined(__DJGPP__) && !defined(__BORLANDC__)
#define MEM_SEG 1
#endif
#endif

#if defined(__clang__)
#define CC_NAME "Clang"
#elif defined(__DJGPP__)
#define CC_NAME "DJGPP"
#elif defined(__GNUC__)
#define CC_NAME "GNU C"
#elif defined(__BORLANDC__)
#define CC_NAME "Borland C"
#elif defined(__WATCOMC__)
#define CC_NAME "Watcom C"
#elif defined(__TURBOC__)
#define CC_NAME "Turbo C"
#elif defined(__HIGHC__)
#define CC_NAME "MetaWare High C"
#elif defined(__IBMC__)
#define CC_NAME "VisualAge C"
#elif defined(_MSC_VER)
#define CC_NAME "MSVC"
#elif defined(__LCC__)
#define CC_NAME "LCC"
#elif defined(__DMC__)
#define CC_NAME "DMC"
#else
#define CC_NAME "Unknown"
#error "CC_NAME not defined. Unknown compiler detected."
#endif

#if defined(OS_DOS)
#ifdef MEM_SEG
#define OS_NAME "DOS 16-bit"
#else
#define OS_NAME "DOS 32-bit"
#endif
#elif defined(OS_WIN)
#if defined(WIN64) || defined(_WIN64)
#define OS_NAME "Win64"
#else
#define OS_NAME "Win32"
#endif
#elif defined(OS_OS2)
#ifdef MEM_SEG
#define OS_NAME "OS/2 16-bit"
#else
#define OS_NAME "OS/2 32-bit"
#endif
#elif defined(__linux__) || defined(__LINUX__)
#define OS_NAME "Linux"
#elif defined(__FreeBSD__)
#define OS_NAME "FreeBSD"
#elif defined(__APPLE__)
#define OS_NAME "Darwin"
#elif defined(__NetBSD__)
#define OS_NAME "NetBSD"
#elif defined(__OpenBSD__)
#define OS_NAME "OpenBSD"
#elif defined(__minix)
#define OS_NAME "Minix"
#else
#error "OS_NAME was not defined. Unknown build target detected."
#define OS_NAME "Unknown"
#endif

#if defined(OS_DOS) || defined(OS_OS2) || defined(OS_WIN)
#define DIRSEP "\\"
#else
#define DIRSEP "/"
#endif

#if defined(OS_UNIX)

#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#define MYMAXFILE  64
#define MYMAXDIR   1024
#define MYMAXPATH  1088
#define MYMAXEXT   12
#define MYMAXDRIVE 3

#define filecmp    strcmp
#define filenodir(x) (strchr(x,'/') == NULL)

#define HAVE_GETPID 1

struct _filefind
{
    char path[MYMAXDIR];
    char mask[MYMAXFILE];
    DIR *dirp;
    struct dirent *pentry;
    int flags;
};

#elif defined(OS_OS2) && defined(__BORLANDC__)

#include <dos.h>
#include <direct.h>
#include <dirent.h>
#include <io.h>
#include <process.h>

#define HAVE_OS_FULLPATH
#define HAVE_OS_FGETS
#define HAVE_STRUPR
#define HAVE_GETPID

#define MYMAXFILE  _MAX_FNAME
#define MYMAXDIR   _MAX_DIR
#define MYMAXPATH  _MAX_PATH
#define MYMAXEXT   _MAX_EXT
#define MYMAXDRIVE _MAX_DRIVE

struct _filefind
{
    char path[MYMAXFILE];
    struct find_t fileinfo;
};

#define filecmp stricmp
#define filenodir(x) (strpbrk(x,"\\/") == NULL)
#define strcasecmp stricmp

/* vsnprintf() unavailable in Borland C */
#define vsnprintf(str, n, fmt, ap) vsprintf(str, fmt, ap)

#elif defined(__WATCOMC__)

#include <dos.h>
#include <direct.h>
#include <io.h>
#include <process.h>

#define MYMAXFILE  _MAX_FNAME
#define MYMAXDIR   _MAX_DIR
#define MYMAXPATH  _MAX_PATH
#define MYMAXEXT   _MAX_EXT
#define MYMAXDRIVE _MAX_DRIVE

#define HAVE_GETPID 1

struct _filefind
{
    char path[MYMAXFILE + MYMAXEXT];
    struct find_t fileinfo;
};

#define filecmp stricmp
#define filenodir(x) (strpbrk(x,"\\/") == NULL)
#define strcasecmp stricmp

#if __WATCOMC__ <= 1100
/* vsnprintf() unavailable in early versions of Watcom C */
#define vsnprintf(str, n, fmt, ap) vsprintf(str, fmt, ap)
#endif

#elif defined(OS_WIN)
#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(__DMC__) || defined(__LCC__)

#include <io.h>
#include <direct.h>
#include <process.h>

#define MYMAXFILE  FILENAME_MAX
#define MYMAXDIR   MYMAXFILE
#define MYMAXPATH  MYMAXFILE
#define MYMAXEXT   13
#define MYMAXDRIVE 3

#define filecmp    stricmp
#define filenodir(x) (strchr(x,'/') == NULL)
#define strcasecmp stricmp

#define HAVE_GETPID 1

struct _filefind
{
    char path[MYMAXDIR];
    int handle;
    struct _finddata_t fileinfo;
};

#if _MSC_VER <= 900
/* vsnprintf() unavailable on very old version of MSVC, so use insecure vsprintf() */
#define vsnprintf(str, n, fmt, ap) vsprintf(str, fmt, ap)
#endif

#endif

#elif defined(OS_DOS)
#if defined(__TURBOC__)

#include <dir.h>
#include <io.h>
#include <process.h>

#define MYMAXFILE  MAXFILE
#define MYMAXDIR   MAXDIR
#define MYMAXPATH  MAXPATH
#define MYMAXEXT   MAXEXT
#define MYMAXDRIVE MAXDRIVE

struct _filefind
{
    char path[MYMAXFILE + MYMAXEXT];
    struct ffblk fileinfo;
};

#define filecmp  stricmp
#define filenodir(x) (strpbrk((x),"\\/") == NULL)
#define strcasecmp stricmp

#define HAVE_GETPID 1

/* vsnprintf() unavailable on very old version of Turbo C, so use insecure vsprintf() */
#define vsnprintf(str, n, fmt, ap) vsprintf(str, fmt, ap)

#elif defined(__DJGPP__)

#include <dir.h>
#include <io.h>
#include <unistd.h>
#include <dirent.h>

#define MYMAXFILE  MAXFILE
#define MYMAXDIR   MAXDIR
#define MYMAXPATH  MAXPATH
#define MYMAXEXT   MAXEXT
#define MYMAXDRIVE MAXDRIVE

struct _filefind
{
    char path[MYMAXDIR];
    char mask[MYMAXFILE];
    DIR *dirp;
    struct dirent *pentry;
    int flags;
};

#define filecmp stricmp
#define filenodir(x) (strpbrk((x),"\\/") == NULL)
#define strcasecmp stricmp

#define HAVE_GETPID 1

/* vsnprintf() unavailable in DJGPP, so use insecure vsprintf() */
#define vsnprintf(str, n, fmt, ap) vsprintf(str, fmt, ap)

#endif
#endif

char *os_findfirst(struct _filefind *pff, const char *path, const char *mask);
char *os_findnext(struct _filefind *pff);
void os_findclose(struct _filefind *pff);
char *os_findfile(struct _filefind *pff, const char *path, const char *mask);

char *os_file_getname(const char *path);
int os_fullpath(char *dst, const char *src, size_t bufsiz);
int os_fulldir(char *dst, const char *src, size_t bufsiz);
int os_chdir(char *path);
char *os_append_slash(char *path);
char *os_remove_slash(char *path);
char *os_dirsep(char *path);
int os_spawn(const char *command, const char *cmdline);
char *os_fgets(char *buf, size_t len, FILE *f);
char *os_getcwd(char *buf, size_t size);

#endif

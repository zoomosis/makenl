/* $Id: os.h,v 1.59 2015/06/11 02:27:24 ozzmosis Exp $ */

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
 *  MEM_FLAT  Flat memory model
 *  MEM_SEG   Segmented memory model (not flat)
 *
 *
 *  Compiler & OS #define strings (informational only)
 *
 *  CC_NAME  Compiler name
 *  OS_NAME  Operating system name
 */

#if defined(MSDOS) || defined(__MSDOS__) || defined(__DOS__)
#define OS_DOS 1
#elif defined(__OS2__) || defined(_OS2) || defined(__EMX__)
#define OS_OS2 1
#elif defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64) || defined(__NT__)
#define OS_WIN 1
#else
#define OS_UNIX 1
#endif

#if defined(OS_DOS)
#if defined(__386__) || defined(__DJGPP__)
#define MEM_FLAT 1
#else
#define MEM_SEG 1
#endif
#endif

#if defined(OS_OS2)
#if defined(__386__) || defined(__BORLANDC__) || defined(__HIGHC__) || defined(__EMX__) || defined(__IBMC__)
#define MEM_FLAT 1
#else
#define MEM_SEG 1
#endif
#endif

#if !defined(MEM_FLAT) && !defined(MEM_SEG)
/* default to flat memory model */
#define MEM_FLAT 1
#endif

#if defined(__clang__)
#define CC_NAME "Clang"
#elif defined(__DJGPP__)
#define CC_NAME "DJGPP"
#elif defined(__EMX__)
#define CC_NAME "EMX"
#elif defined(__MINGW64__)
#define CC_NAME "MinGW64"
#elif defined(__MINGW32__)
#define CC_NAME "MinGW32"
#elif defined(__ICC)
#define CC_NAME "Intel C"
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
#if _MSC_VER <= 900
#define CC_NAME "MSC"
#else
#define CC_NAME "MSVC"
#endif
#elif defined(__LCC__)
#define CC_NAME "LCC"
#elif defined(__DMC__)
#define CC_NAME "DMC"
#elif defined(__TINYC__)
#define CC_NAME "TinyCC"
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
#define OS_NAME "OS X"
#elif defined(__NetBSD__)
#define OS_NAME "NetBSD"
#elif defined(__OpenBSD__)
#define OS_NAME "OpenBSD"
#elif defined(__sun)
#define OS_NAME "Solaris"
#elif defined(__minix)
#define OS_NAME "Minix"
#elif defined(__HAIKU__)
#define OS_NAME "Haiku"
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

#define NEED_SNPRINTF 1

#elif defined(OS_DOS) && defined(_MSC_VER)

#include <dos.h>
#include <io.h>
#include <direct.h>
#include <process.h>

#define MYMAXFILE  _MAX_FNAME
#define MYMAXDIR   _MAX_DIR
#define MYMAXPATH  _MAX_PATH
#define MYMAXEXT   _MAX_EXT
#define MYMAXDRIVE _MAX_DRIVE

#define filecmp    stricmp
#define filenodir(x) (strchr(x,'/') == NULL)

#define strcasecmp stricmp

#define HAVE_GETPID 1

struct _filefind
{
    char path[MYMAXFILE];
    struct find_t fileinfo;
};

#define NEED_SNPRINTF 1

#elif defined(__WATCOMC__) || defined(__HIGHC__)

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

#if __WATCOMC__ <= 1100
#define NEED_SNPRINTF 1
#endif

struct _filefind
{
    char path[MYMAXFILE + MYMAXEXT];
    struct find_t fileinfo;
};

#define filecmp stricmp
#define filenodir(x) (strpbrk(x,"\\/") == NULL)
#define strcasecmp stricmp

#elif defined(OS_OS2) && defined(__IBMC__)

#include <direct.h>
#include <io.h>
#include <process.h>

#define MYMAXFILE  _MAX_FNAME
#define MYMAXDIR   _MAX_DIR
#define MYMAXPATH  _MAX_PATH
#define MYMAXEXT   _MAX_EXT
#define MYMAXDRIVE _MAX_DRIVE

#define NAME_MAX 255

struct find_t
{
    char reserved[21];
    char attrib;
    unsigned short wr_time;
    unsigned short wr_date;
    unsigned long size;
    char name[NAME_MAX + 1];
};

struct _filefind
{
    char path[MYMAXFILE + MYMAXEXT];
    struct find_t fileinfo;
};

#define filecmp stricmp
#define filenodir(x) (strpbrk(x,"\\/") == NULL)
#define strcasecmp stricmp

#define HAVE_GETPID 1
#define NEED_SNPRINTF 1

#elif defined(OS_WIN)
#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(__DMC__) || defined(__LCC__) || defined(__MINGW32__)

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

#ifndef __MINGW64__
#define strcasecmp stricmp
#endif

#define HAVE_GETPID 1

struct _filefind
{
    char path[MYMAXDIR];
    int handle;
    struct _finddata_t fileinfo;
};

#if _MSC_VER <= 1500
#define NEED_SNPRINTF 1
#endif

#ifdef __DMC__
#define NEED_SNPRINTF 1
#define NEED_SNPRINTF_ONLY 1
#endif

#endif

#elif defined(OS_DOS) && defined(__TURBOC__)

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
#define NEED_SNPRINTF 1

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
#define NEED_SNPRINTF 1

#elif defined(__EMX__)

#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#define MYMAXFILE  _MAX_FNAME
#define MYMAXDIR   _MAX_DIR
#define MYMAXPATH  _MAX_PATH
#define MYMAXEXT   _MAX_EXT
#define MYMAXDRIVE _MAX_DRIVE

#define HAVE_GETPID 1

#define filecmp  stricmp
#define filenodir(x) (strpbrk(x,"\\/") == NULL)
#define strcasecmp stricmp

struct _filefind
{
    char path[MYMAXDIR];
    char mask[MYMAXFILE];
    DIR *dirp;
    struct dirent *pentry;
    int flags;
};

#endif

#ifndef NEED_SNPRINTF
#define HAVE_SNPRINTF 1
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

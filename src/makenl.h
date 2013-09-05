/* $Id: makenl.h,v 1.8 2013/09/05 15:07:51 ozzmosis Exp $ */

#ifndef __MAKENL_H__
#define __MAKENL_H__

#include "os.h"

/* Variables from makenl.c that are needed at some places */

extern char *WorkFile;
extern const char *const DOWLongnames[7];
extern const char *const MonthLongnames[12];
extern int MakeType;
extern int MakeNum;
extern int ExitCode;
extern int NewExtWDay;
extern int JustTest;
extern int debug_mode;

#define linelength 512
#define ARCCMDMAX 20
#define ARCEXTMAX 4
#define ARCUNPMAX 10

/* ARCUNPMAX must be set to the same nr of archivers in unpacker() in lsttool.c */

extern char ArcCopyExt[ARCEXTMAX];
extern char ArcMoveExt[ARCEXTMAX];
extern char ArcOpenExt[ARCUNPMAX][ARCEXTMAX];
extern char ArcCopyCmd[ARCCMDMAX];
extern char ArcMoveCmd[ARCCMDMAX];
extern char ArcOpenCmd[ARCUNPMAX][ARCCMDMAX];
extern int  ArcOpenCnt;

/* the die function from makenl.c */
void die(int exitcode, const char *format, ...);

/* testing() in testing.c */

void testing(void);

#ifndef max
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#ifdef MALLOC_DEBUG
#include "rmalloc.h"
#endif

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#endif

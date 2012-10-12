/* $Id: makenl.h,v 1.2 2012/10/12 22:54:00 ozzmosis Exp $ */

#include "os.h"

/* Variables from MAKENL.C that are needed at some places */

extern char *WorkFile;
extern const char *const DOWLongnames[7];
extern const char *const MonthLongnames[12];
extern int MakeType;
extern int MakeNum;
extern int ExitCode;
extern int NewExtWDay;
extern int JustTest;
extern int MakenlDebug;

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

#define Debug1(msg, arg) if (MakenlDebug) fprintf(stderr, "DEBUG: " FUNCNAME ": " msg "\n", arg)
#define Debug(msg)       if (MakenlDebug) fprintf(stderr, "DEBUG: " FUNCNAME ": " msg "\n")

/* the die-Function from MAKENL.C */
void die(int exitcode, int on_stderr, const char *format, ...);

/* the utility-Functions from STRTOOL.C */
int getnumber(const char *string, int *output);
char *skipspaces(char *ptr);
char *cutspaces(char *string);

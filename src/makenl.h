/* $Id: makenl.h,v 1.4 2012/10/14 14:07:22 ozzmosis Exp $ */

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

/* the die function from makenl.c */
void die(int exitcode, const char *format, ...);

/* the utility functions from strtool.c */
int getnumber(const char *string, int *output);
char *skipspaces(char *ptr);
char *cutspaces(char *string);

/* necessary for some C implementations where printf("%s\n", NULL) would cause a segfault */

#define make_str_safe(x) (x)?(x):"(null)"

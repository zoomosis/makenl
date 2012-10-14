/* $Id: makenl.h,v 1.5 2012/10/14 14:49:17 ozzmosis Exp $ */

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

/* the utility functions from strtool.c */
int getnumber(const char *string, int *output);
char *skipspaces(char *ptr);
char *cutspaces(char *string);

/* necessary for some C implementations where printf("%s\n", NULL) would cause a segfault */

#define make_str_safe(x) (x)?(x):"(null)"

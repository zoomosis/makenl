/* $Id: makenl.h,v 1.7 2004/07/19 14:20:52 ozzmosis Exp $ */

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

extern char ArcExt[4];
extern char ArcCopyCmd[ARCCMDMAX];
extern char ArcMoveCmd[ARCCMDMAX];
extern char ArcOpenCmd[ARCCMDMAX];

#define Debug1(msg, arg) if (MakenlDebug) fprintf(stderr, "DEBUG: " FUNCNAME ": " msg "\n", arg)
#define Debug(msg)       if (MakenlDebug) fprintf(stderr, "DEBUG: " FUNCNAME ": " msg "\n")

/* the die-Function from MAKENL.C */
void die(int exitcode, int on_stderr, const char *format, ...);

#define die_if_file(f, fn, output) if(!f) die(0xFE, 1, "Unable to open \"%s\" for %s\n", fn, output?"Output":"Input");

/* the utility-Functions from STRTOOL.C */
int getnumber(const char *string, int *output);
char *skipspaces(char *ptr);
char *cutspaces(char *string);

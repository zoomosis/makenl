/* $Id: lsttool.h,v 1.3 2004/07/11 09:32:05 ozzmosis Exp $ */

#define SEARCH_UPLOAD 0
#define SEARCH_MAILFILE 1
#define SEARCH_UPDATE 2
#define SEARCH_MASTER 3
#define SEARCH_NOWHERE 4

#define CAUSE_THRESHOLD 1
#define CAUSE_OUTDIFF 2

extern long ARCThreshold;
extern long DIFFThreshold;
extern int CopyrightLines;      /* Used for diffing - copyright appears in 
                                   each diff */

int makearc(char *filename, int move);
int installlist(char *filename, char *extbfr);
int openlist(FILE ** listFILEptr, char *filename, char *foundfile,
             int where, int mustbenew);
int makediff(char *filename);

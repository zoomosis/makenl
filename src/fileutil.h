/* $Id: fileutil.h,v 1.6 2004/09/02 21:17:39 mbroek Exp $ */

#include "os.h"

extern char MakeSourceFile[MYMAXFILE + MYMAXEXT];
extern char OutFile[MYMAXFILE + MYMAXEXT];
extern char OutDiff[MYMAXFILE + MYMAXEXT];
extern char CopyrightFile[MYMAXFILE + MYMAXEXT];
extern char PrologFile[MYMAXFILE + MYMAXEXT];
extern char EpilogFile[MYMAXFILE + MYMAXEXT];
extern char MergeFilename[MYMAXPATH];
extern char CommentsFile[MYMAXPATH];

extern char CurDir[MYMAXDIR];
extern char OutDir[MYMAXDIR];
extern char MasterDir[MYMAXDIR];
extern char UpdateDir[MYMAXDIR];
extern char MessageDir[MYMAXDIR];
extern char MailfileDir[MYMAXDIR];
extern char UploadDir[MYMAXDIR];
extern char BadDir[MYMAXDIR];

extern char BatchFile[MYMAXPATH];
extern char CalledBatchFile[MYMAXFILE];
extern char LogFile[MYMAXPATH];

int GetPath(char *arg, int switchno);

 /* returns length of extension 0..3 */
int getext(char *ext, char *filename);
void swapext(char *newname, const char *origname, const char *newext);
void myfnmerge(char *output, const char *drive, const char *dir,
               const char *name, const char *ext);
void myfnsplit(const char *input, char *drive, char *dir, char *name,
               char *ext);
long filesize(const char *filename);
extern int do_clean;
extern char OldExtensions[4][MYMAXEXT];

void cleanit(void);
void cleanold(char *path, char *filename, char *ext);
void CopyOrMove(int copy, char *source, char *destdir, char *destname);

/* Now in OS.H as a #define 
int filecmp (const char *a, const char *b);*/

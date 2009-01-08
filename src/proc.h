/* $Id: proc.h,v 1.1.1.1 2009/01/08 20:07:47 mbroek Exp $ */

/* Values for ShouldProcess */
#define USUAL_PROCESSING 1
#define FORCED_PROCESSING 2

extern int ShouldProcess;
extern int SubmitFile;
extern int IsLinux;

void ProcessFILES(int WorkMode, FILE * CfgFILE, FILE * OutFILE,
                  FILE * CommentsFILE, FILE * MergeOutFILE,
                  unsigned short *crc);

int processfile(int myMakeType, int myMakeNum, FILE * InputFILE,
                FILE * OutFILE, FILE * FooFile, FILE * MergOutFILE,
                FILE * SelfMsgFILE, unsigned short *OutCRC, int *WorkMode);

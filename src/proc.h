/* $Id: proc.h,v 1.4 2004/07/11 09:32:05 ozzmosis Exp $ */

/* Values for ShouldProcess */
#define USUAL_PROCESSING 1
#define FORCED_PROCESSING 2

extern int ShouldProcess;
extern int SubmitFile;

void ProcessFILES(int WorkMode, FILE * CfgFILE, FILE * OutFILE,
                  FILE * CommentsFILE, FILE * MergeOutFILE,
                  unsigned short *crc);

int processfile(int myMakeType, int myMakeNum, FILE * InputFILE,
                FILE * OutFILE, FILE * FooFile, FILE * MergOutFILE,
                FILE * SelfMsgFILE, unsigned short *OutCRC, int *WorkMode);

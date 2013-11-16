/* $Id: procfile.h,v 1.1 2013/09/21 12:09:41 ozzmosis Exp $ */

#ifndef __PROCFILE_H__
#define __PROCFILE_H__

/* Values for ShouldProcess */

#define USUAL_PROCESSING 1
#define FORCED_PROCESSING 2

extern int ShouldProcess;
extern int SubmitFile;

void ProcessFILES(int WorkMode, FILE * CfgFILE, FILE * OutFILE,
  FILE * CommentsFILE, FILE * MergeOutFILE, unsigned short *crc);

int processfile(int myMakeType, int myMakeNum, FILE * InputFILE,
  FILE * OutFILE, FILE * FooFile, FILE * MergOutFILE, FILE * SelfMsgFILE,
  unsigned short *OutCRC, int *WorkMode);

#endif

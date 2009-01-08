/* $Id: msg.h,v 1.1 2009/01/08 20:07:47 mbroek Exp $ */

#define MF_SHIFT_ERRORS 4
#define MF_SHIFT_SUBMIT 8

#define MF_RECEIPT 0x0F
#define MF_ERRORS (MF_RECEIPT << MF_SHIFT_ERRORS)
#define MF_SUBMIT (MF_RECEIPT << MF_SHIFT_SUBMIT)
#define MF_SELF 0x1000

#define MF_DOIT 0x1
#define MF_CRASH 0x2
#define MF_HOLD 0x4
#define MF_INTL 0x8

extern int UsualMSGFlags;
extern int MailerFlags;
extern int NotifyAddress[3];
extern int SubmitAddress[3];
extern int MyAddress[3];

int ParseAddress(const char *string, int *out);
FILE *OpenMSGFile(int *adress, char *filename);
FILE *CloseMSGFile(int status);

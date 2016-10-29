/* $Id: config.h,v 1.5 2016/10/29 06:58:12 ajleary Exp $ */

/* Numbers of configuration options */
#define CFG_ARCCOPY 0
#define CFG_ARCMOVE 32
#define CFG_ARCOPEN 33
#define CFG_BADFILES 1
#define CFG_BAUDRATE 2
#define CFG_CLEANUP 3
#define CFG_COMMENTS 4
#define CFG_COPYRIGHT 5
#define CFG_DATA 6
#define CFG_EPILOG 7
#define CFG_FILES 8
#define CFG_MAILFILES 9
#define CFG_MAKE 10
#define CFG_MASTER 11
#define CFG_MERGE 12
#define CFG_MESSAGES 13
#define CFG_MINPHONE 14
#define CFG_NAME 15
#define CFG_NETADDRESS 16
#define CFG_NOTIFY 17
#define CFG_OUTDIFF 18
#define CFG_OUTFILE 19
#define CFG_OUTPATH 20
#define CFG_PROCESS 21
#define CFG_PROLOG 22
#define CFG_PUBLISH 23
#define CFG_PRIVATE 24
#define CFG_SUBMIT 25
#define CFG_THRESHOLD 26
#define CFG_UPDATE 27
#define CFG_UPLOADS 28
#define CFG_POINTS 29
#define CFG_BATCHFILE 30
#define CFG_CALLEDBATCHFILE 31

/* #define CFG_ARCMOVE 32   # moved to second place */
/* #define CFG_ARCOPEN 33   # moved to third place */

/* Allow Alpha Character in Field Six */
/*    - requested by Michael Grant    */

#define CFG_ALPHAPHONE 34

/* Allow UnPublished w/o Pvt */

#define CFG_ALLOWUNPUB 35

/* Logfile */
#define CFG_LOGFILE 36
#define CFG_LOGLEVEL 37

/* Force submit unchanged file */
#define CFG_FORCESUBMIT 38

/* Allow 8-bit characters */
#define CFG_ALLOW8BIT 39

/* Valid baudrates */
#define MAX_BAUDRATES 12

/* Remove UTF-8 Byte Order Marks (BOM) */
#define CFG_REMOVEBOM 40

struct switchstruct
{
    char *name;
    int minlen;
    int value;
};

extern const struct switchstruct MakeTypes[];

void DoCmdLine(char **argv, char **cfgfilename);
int parsecfgfile(FILE * CFG);
unsigned int xlate_switch(const char *swit,
                          const struct switchstruct *desc);

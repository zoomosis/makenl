/* $Id: fts5.h,v 1.5 2004/07/12 21:59:57 fido Exp $ */

/* Indices for 3D-Adresses */
#define A_ZONE 0
#define A_NET 1
#define A_NODE 2
#define A_POINT 3

enum level_enum
{
    LEVEL_TOP, LEVEL_ZONE, LEVEL_REGION, LEVEL_HOST, LEVEL_HUB,
    LEVEL_NODE, LEVEL_OGATE, LEVEL_PRIVATE, LEVEL_DOWN, LEVEL_HOLD,
    LEVEL_POINT, LEVEL_COMMENT, LEVEL_EMPTY
};

/* maximal *length* of phone numbers */
#define MAXPHONE 30

/* minimal *parts* of phone numbers */
extern int Minphone;

/* Allow Alpha Character in Phone */
extern int Alphaphone;

/* Allow -Unpublished w/o Pvt */
extern int Allowunpub;

extern char namebuf[16];
extern const char *const LevelsSimple[];
extern char *Levels[];
extern char *FTS5Line[8];
extern const int Level4DPos[];
extern int PrivateLevel;
extern int PointLevel;
extern char ErrorMessage[linelength];

#define FTS5Keyword FTS5Line[0]
#define FTS5Number FTS5Line[1]
#define FTS5Nodename FTS5Line[2]
#define FTS5Location FTS5Line[3]
#define FTS5Sysopname FTS5Line[4]
#define FTS5Phone FTS5Line[5]
#define FTS5Baud FTS5Line[6]
#define FTS5Flags FTS5Line[7]

int GetBaudrateTable(const char *list);
int getnodenum(char **instring, int *linelevel, int *linenum);
int ParseFTS5(char *line, int *a, int *b);
int OutputErrorLine(FILE * where, const char *pre, const char *wrongy,
                    const char *post, unsigned short *crc);
int OutputFTS5Line(FILE * file, const char *prefix, const char *postfix,
                   unsigned short *crc);
int CopyComment(FILE * output, char *Copyfile, const char *year,
                unsigned short *crc);

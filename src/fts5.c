/* $Id: fts5.c,v 1.13 2005-07-22 22:29:13 fido Exp $ */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "config.h"
#include "makenl.h"
#include "fts5.h"
#include "unused.h"

#ifdef MALLOC_DEBUG
#include "rmalloc.h"
#endif

#ifdef DMALLOC
#include "dmalloc.h"
#endif

extern int nl_baudrate[];

static struct switchstruct FTS5KWords[] = {
    {"", 0, 5},
    {"GLOBAL", 6, LEVEL_TOP},
    {"ZONE", 4, LEVEL_ZONE},
    {"REGION", 6, LEVEL_REGION},
    {"HOST", 4, LEVEL_HOST},
    {"HUB", 3, LEVEL_HUB},
    {"PRIVATE", 7, LEVEL_PRIVATE},
    {"PVT", 3, LEVEL_PRIVATE},
    {"OGATE", 5, LEVEL_OGATE},
    {"DOWN", 4, LEVEL_DOWN},
    {"HOLD", 4, LEVEL_HOLD},
    {"POINT", 5, LEVEL_POINT},
    {"KENNEL", 6, LEVEL_DOWN},
    {"KENL", 4, LEVEL_DOWN},
    {NULL, 0, -1}
};

const int Level4DPos[] = {
/* LEVEL_TOP   --> */ A_ZONE,
/* LEVEL_ZONE  --> */ A_ZONE,
/* LEVEL_REGION--> */ A_NET,
/* LEVEL_HOST  --> */ A_NET,
/* LEVEL_HUB   --> */ A_NODE,
/* LEVEL_NODE  --> */ A_NODE,
/* LEVEL_OGATE --> */ A_NODE,
/* LEVEL_PRIVATE-> */ A_NODE,
/* LEVEL_DOWN  --> */ A_NODE,
/* LEVEL_HOLD  --> */ A_NODE,
/* LEVEL_POINT --> */ A_POINT,
/* LEVEL_COMMENT-> */ 127,
/* LEVEL_COMMENT-> */ 127
};
const char *const LevelsSimple[] =
    { "", "Zone", "Region", "Network", "Hub", "Node" };
char namebuf[16];

/* Original */

/* char *Levels[] =
    { namebuf, "Zone", "Region", "Host", "Hub", "", "Ogate", "Pvt", "Down",
    "Hold", "Point"
};
*/

char *Levels[] =
  { namebuf, "Zone", "Region", "Host", "Hub", "", "Hold", "Down", "Ogate",
  "Pvt", "Point"
};

int Minphone = 3;
int Alphaphone = 0;
int Allowunpub = 0;
int PrivateLevel;
int PointLevel = -2;            /* don't allow points */
char *FTS5Line[8];

static void Space2Under(char *string)
{
    for (; *string; string++)
        if (*string == ' ')
            *string = '_';
}

static int getkeyword(char **instring, int *linelevel, int *linenum)
{
    unused(linenum);

    if ((*linelevel = xlate_switch(strupr(*instring), FTS5KWords)) == -1)
    {
        sprintf(ErrorMessage, "Invalid keyword -- \"%s\"", *instring);
        return 1;
    }

    return 0;
}

int getnodenum(char **instring, int *linelevel, int *linenum)
{
    int addrlen;
    const char *type;

    addrlen = strlen(*instring);
    if (addrlen != 0)
        if (getnumber(*instring, linenum) == addrlen)
            if (*linenum != 0)
            {
                while (**instring == '0')
                    ++(*instring);
                return 0;
            }

    if (*linelevel == LEVEL_POINT)
    {
        type = "Point";
    }
    else if (*linelevel > LEVEL_HUB)
    {
        type = "Node";
    }
    else
    {
        if (*linelevel == -1)
        {
            type = "Node";
        }
        else
        {
            type = LevelsSimple[*linelevel];
        }
    }
    sprintf(ErrorMessage, "Invalid %s number -- \"%s\"", type, *instring);
    return 1;
}

static int getstring(char **instring, int *linelevel, int *linenum)
{
    char *workptr;

    unused(linelevel);
    unused(linenum);

    workptr = *instring;
    if (strlen(workptr) != 0)
    {
        Space2Under(workptr--);
        while (*(++workptr) != 0)
        {
            if (*workptr & 0x80)
                *workptr = '?';
        }
    }
    else
        *instring = "-Unknown-";

    return 0;
}

static int getphone(char **instring, int *linelevel, int *linenum)
{
    int parts;
    int digit_seen;
    char *phoneno;
    char *phonewalk;
    char phonechar;
    int brackets;

    unused(linenum);

/* MGMMGM Pass through any -Unpublished- phone number without further checking */
/*        Standardise capitalisation */

    if (Allowunpub == 1)
        if (stricmp(*instring, "-unpublished-") == 0)
        {
            *instring = "-Unpublished-";
            return 0;
        }
    if (Alphaphone == 1 && (stricmp(*instring, "-Unpublished-") != 0))
        return 0;

/* end MGMMGM */

    switch (*linelevel)
    {
    case LEVEL_PRIVATE:
        *instring = "-Unpublished-";
        break;
    case LEVEL_HOLD:
    case LEVEL_DOWN:
        /* Hold and down nodes don't need a valid number */
        break;
    case LEVEL_POINT:
        /* Points may have unpublished phones */
        if (!stricmp(*instring, "-Unpublished-"))
            return 0;
    default:
        phoneno = *instring;
        phonewalk = phoneno;
        brackets = 0;
        digit_seen = 0;
        parts = 0;
        do
        {
            phonechar = *phonewalk;
            phonewalk++;
            if (isdigit((unsigned char)phonechar))
                digit_seen = 1;
            else if (digit_seen != 0)
            {
                switch (phonechar)
                {
                  BadPhone:
                default:
                    sprintf(ErrorMessage, "Invalid phone number -- \"%s\"",
                            phoneno);
                    return 1;
                case '[':
                    if (parts + 1 < Minphone)
                        goto FewParts;
                    parts = -1;
                case ']':
                    brackets++;
                case 0:
                case '-':
                    digit_seen = 0;
                    parts++;
                }
            }
            else if (!(phonechar == '\0' && brackets == 2)) /* Allow \0
                                                               directly
                                                               after ] */
                goto BadPhone;
        }
        while (phonechar);
        if (parts < Minphone)
        {
          FewParts:
            sprintf(ErrorMessage,
                    "Phone number \"%s\" has fewer than %d parts", phoneno,
                    Minphone);
            return 1;
        }
        break;
    }

    return 0;
}

static int getbaud(char **instring, int *linelevel, int *linenum)
{
    int e_len, i, good;
    char *baud_no;

    unused(linelevel);
    unused(linenum);

    e_len = 0;
    good = 0;
    baud_no = *instring;

    while ((unsigned char)baud_no[e_len] != 0)
    {
        if (!isdigit((unsigned char)baud_no[e_len]))
        {
            sprintf(ErrorMessage, "Invalid baud rate -- \"%s\"",
                    *instring);
            return 1;
        }
        e_len++;
    }

    if (e_len == 0)
    {
	sprintf(ErrorMessage, "Invalid baud rate -- \"%s\"", *instring);
	return 1;
    }

    /* Check with table */
    for (i = 0; i < MAX_BAUDRATES; i++)
    {
	if (nl_baudrate[i] == 0)
	    break;
	if (nl_baudrate[i] == atoi(*instring))
	{
	    good = 1;
	    break;
	}
    }

    if (good == 0)
    {
	sprintf(ErrorMessage, "Invalid baud rate -- \"%s\"", *instring);
	return 1;
    }

    return 0;
}

typedef int (*handlefunc) (char **instring, int *linelevel, int *linenum);
static handlefunc HandleFields[] =
    { getkeyword, getnodenum, getstring, getstring, getstring, getphone,
    getbaud
};

int ParseFTS5(char *line, int *a, int *b)
{
    int fieldno;
    int hiterror;
    char **ElementPPtr = &FTS5Keyword;
    handlefunc *y = HandleFields;

    if (strlen(cutspaces(line)) == 0)
    {
        *a = LEVEL_EMPTY;
        return 0;
    }
    if (line[0] == ';')
    {
        *a = LEVEL_COMMENT;
        return 0;
    }
    hiterror = 0;
    for (fieldno = 0; *ElementPPtr = skipspaces(line), fieldno < 7;
         fieldno++, ElementPPtr++)
    {
        line += strcspn(line, ",\r\n");
        if (*line != 0)
            *(line++) = 0;
        cutspaces(*ElementPPtr);
        if (!hiterror)
            hiterror = (*(y++)) (ElementPPtr, a, b);
    }
    if (!hiterror)
        FTS5Keyword = Levels[*a];
    return hiterror;
}

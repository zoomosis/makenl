/* $Id: upcont.c,v 1.6 2004/07/15 17:44:02 ozzmosis Exp $ */

#include <string.h>
#include <stdio.h>

#include "makenl.h"
#include "upcont.h"
#include "fts5.h"

#ifdef MALLOC_DEBUG
#include "rmalloc.h"
#endif

#ifdef DMALLOC
#include "dmalloc.h"
#endif

/* *INDENT-OFF* */
/* Don't indent this array, keep it this way (you could try what indent
   makes of it. It is not really fun, its just funny */
static int OnTop[] = {
/* LEVEL_TOP   --> */ LEVEL_TOP,
/* LEVEL_ZONE  --> */ LEVEL_TOP,
/* LEVEL_REGION--> */ LEVEL_ZONE,
/* LEVEL_HOST  --> */ LEVEL_REGION,
/* LEVEL_HUB   --> */ LEVEL_HOST,
/* LEVEL_NODE  --> */ LEVEL_TOP,   /* Nodes may be anywhere */
/* LEVEL_OGATE --> */ LEVEL_HOST,
/* LEVEL_PRIVATE-> */ LEVEL_HOST,
/* LEVEL_HOLD  --> */ LEVEL_TOP,   /* Hold and down nodes may be anywhere */
/* LEVEL_DOWN  --> */ LEVEL_TOP,
/* LEVEL_POINT --> */ LEVEL_HUB,   /* Hubs and nodes may have points */
};
/* *INDENT-ON* */


static char Node[] = "Node\0";
static char PVT[] = ",PVT";
static char PhoneNoHolder[MAXPHONE];
static char PointPhoneNoHolder[MAXPHONE];

int
UpdateContext(int level, int num, int makenum, int *ctxnum, int *ctxlevel,
              int maketype)
{
    const char *leveltxt;
    const char *makeleveltxt;

    if (makenum)
    {
        if ((level >= maketype - 1)
            && (maketype == LEVEL_NODE || maketype == level)
            && makenum == num)
            goto out;
        if (level == LEVEL_POINT)
            leveltxt = "Point";
        else if (level >= LEVEL_NODE)
            leveltxt = Node;
        else
            leveltxt = Levels[level];
        if (maketype >= LEVEL_NODE)
            makeleveltxt = Node;
        else
            makeleveltxt = Levels[maketype];
        sprintf(ErrorMessage, "Expected \"%s %d\", got \"%s %d\"",
                makeleveltxt, makenum, leveltxt, num);
        return 2;
    }
    if (maketype > level)
    {
        sprintf(ErrorMessage, "Unexpected \"%s\" statement in %s file",
                Levels[level], LevelsSimple[maketype]);
        return 2;
    }
    if (level == maketype && maketype != LEVEL_NODE)
    {
        sprintf(ErrorMessage, "Multiple \"%s\" statememts",
                (maketype > LEVEL_HUB) ? "Node" : Levels[maketype]);
        return 2;
    }
    switch (level)
    {
    case LEVEL_POINT:
        if (*ctxlevel > LEVEL_NODE) /* Catch private, hold and down nodes
                                       with points */
        {
            sprintf(ErrorMessage, "%s nodes may not have points\n",
                    Levels[*ctxlevel]);
            return 1;
        }
        switch (PointLevel)
        {
        case 0:                /* OK */
            break;
        case -2:               /* NONE */
            strcpy(ErrorMessage, "Points not allowed in this list");
            return 1;
        case -3:               /* HIDE */
            FTS5Phone = "-Unpublished-";
            break;
        default:               /* some level - currently only LEVEL_NODE
                                   possible */
            if (PointPhoneNoHolder[0] == 0)
            {
                strcpy(ErrorMessage,
                       "No phone number translation available");
                return 1;
            }
            if (!strcmp(FTS5Phone, "-Unpublished-"))
                FTS5Phone = PointPhoneNoHolder;
        }
        goto check_level;
    case LEVEL_PRIVATE:
        switch (PrivateLevel)
        {
        case 0:                /* OK */
            break;
        case -2:               /* NONE */
            strcpy(ErrorMessage,
                   "Private nodes not allowed in this network");
            return 1;
        default:
            if (PhoneNoHolder[0] == 0)
            {
                strcpy(ErrorMessage,
                       "No phone number translation available");
                return 1;
            }
            FTS5Phone = PhoneNoHolder;
            if (FTS5Flags[0] != 0)
                strcat(FTS5Flags, PVT);
            else
                FTS5Flags = PVT + 1;
            return 0;
        }
        /* FALLTHROUGH */
    case LEVEL_REGION:
    case LEVEL_HOST:
    case LEVEL_HUB:
    case LEVEL_OGATE:
    case LEVEL_NODE:
    case LEVEL_DOWN:
    case LEVEL_HOLD:
      check_level:
        if (OnTop[level] > *ctxlevel)
        {
            sprintf(ErrorMessage, "\"%s\" not allowed in a %s",
                    Levels[level], LevelsSimple[*ctxlevel]);
            if (level > LEVEL_HOST)
                return 1;
            else
                return 2;
        }
      out:
        if (level <= PrivateLevel)
            strcpy(PhoneNoHolder, FTS5Phone);
        if (level <= PointLevel)
            strcpy(PointPhoneNoHolder, FTS5Phone);
        if (level < LEVEL_POINT) /* Points don't start a context... */
        {
            *ctxnum = num;
            *ctxlevel = level;
        }
    }
    return 0;
}

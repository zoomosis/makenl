#include <stdio.h>
#include <stdlib.h>

#if defined(__WATCOMC__)
#include <malloc.h>
#endif

#include "makenl.h"
#include "os.h"
#include "fts5.h"
#include "msg.h"
#include "stack.h"
#include "snprintf.h"
#include "mklog.h"

static int *NumberStackPtr = NULL;
static int NStackWritePtr, NStackReadPtr, NumberListValid, NStackNotFull =
    1, NumberStackLen;

static struct StackPointers
{
    int ReadPtr;
    int WritePtr;
}
NStackPtrs[4];                  /* Zero initialized automatically */
static char *Describe4D[] =
    { "Zone", "Region or Network", "Node", "Point" };

static int MarkedNSWP, MarkedNSRP, MarkedNLV;

int addnumber(int maketype, int makenum, int markpos)
{
    int newlevels;
    int *numberptr;
    int status = 0;
    int num4Dpos;
    struct StackPointers *sptr;
    int fillmin, fillmax, aktpos;

    num4Dpos = Level4DPos[maketype];
    if (num4Dpos < NumberListValid) /* finished one Net or Node */
    {
        NumberListValid = num4Dpos;
        NStackReadPtr = NStackPtrs[num4Dpos].ReadPtr;
        NStackWritePtr = NStackPtrs[num4Dpos].WritePtr;
    }
    else if (num4Dpos > NumberListValid) /* need to create new level(s) */
    {
        sptr = NStackPtrs + NumberListValid;
        newlevels = Level4DPos[maketype] - NumberListValid;
        NumberListValid += newlevels;
        fillmin = NStackReadPtr;
        fillmax = NStackWritePtr;
        do
        {
            sptr->ReadPtr = fillmin;
            sptr->WritePtr = fillmax;
            sptr++;
            fillmin = fillmax;  /* Levels in between have no entries */
        }
        while (--newlevels);
        NStackReadPtr = fillmin;
    }
    if (NStackReadPtr < NStackWritePtr)
    {
        aktpos = NStackReadPtr;
        numberptr = NumberStackPtr + aktpos;
        do
        {
            if (*numberptr == makenum)
            {
                snprintf(ErrorMessage, sizeof ErrorMessage, "Duplicate %s number -- '%d'",
                        Describe4D[NumberListValid], makenum);
                if (NumberListValid >= A_NODE)
                    status = 1;
                else
                    status = 2;
                break;
            }
            numberptr++;
            aktpos++;
        }
        while (aktpos < NStackWritePtr);
    }
    if (markpos)
    {
        MarkedNLV = NumberListValid;
        MarkedNSRP = NStackReadPtr;
        MarkedNSWP = NStackWritePtr;
    }
    if (NStackWritePtr == NumberStackLen && NStackNotFull)
    {
        numberptr = realloc(NumberStackPtr, (NumberStackLen + 100) * sizeof(int));

        if (numberptr != NULL)
        {
            NumberStackPtr = numberptr;
            NumberStackLen += 100;
        }
        else
        {
            mklog(LOG_ERROR, "WARNING -- Unable to allocate more space for number stack");
            mklog(LOG_ERROR, "Duplicate number checking limited to %d numbers", NumberStackLen);
            NStackNotFull = 0;
        }
    }
    if (NStackWritePtr < NumberStackLen)
        NumberStackPtr[NStackWritePtr++] = makenum;
    return status;
}

void unmarkstack(void)
{
    NumberListValid = MarkedNLV;
    NStackReadPtr = MarkedNSRP;
    NStackWritePtr = MarkedNSWP;
}

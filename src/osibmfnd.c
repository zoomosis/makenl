/* $Id: osibmfnd.c,v 1.2 2013/09/13 13:25:44 ozzmosis Exp $ */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define INCL_BASE
#define INCL_DOS
#include <os2.h>

#define HAVE_OS_FIND
#include <string.h>
#include "unused.h"

/* file attribute constants for attr field of _dos_findfirst() */

#define _A_NORMAL 0x00    /* normal file, read/write permitted */
#define _A_RDONLY 0x01    /* read-only file */
#define _A_HIDDEN 0x02    /* hidden file */
#define _A_SYSTEM 0x04    /* system file */
#define _A_VOLID  0x08    /* volume label */
#define _A_SUBDIR 0x10    /* subdirectory */
#define _A_ARCH   0x20    /* archive file */

#define BAD_HANDLE         ((HDIR)(~0))
#define HANDLE_OF(__find)  (*(HDIR *)(&(__find)->reserved[0]))

#define FF_LEVEL   1
#define FF_BUFFER  FILEFINDBUF3

struct name
{
    char buf[NAME_MAX + 1];
};

static void copydir(struct find_t *buf, FF_BUFFER *dir_buff)
{
    buf->attrib = dir_buff->attrFile;
    buf->wr_time = *(unsigned short *)&dir_buff->ftimeLastWrite;
    buf->wr_date = *(unsigned short *)&dir_buff->fdateLastWrite;
    buf->size = dir_buff->cbFile;

    *(struct name *)buf->name = *(struct name *)dir_buff->achName;
}

static unsigned _dos_findfirst( const char *path, unsigned attr, struct find_t *buf)
{
    APIRET rc;
    FF_BUFFER dir_buff;
    HDIR handle = BAD_HANDLE;
    ULONG searchcount;

    searchcount = 1;        /* only one at a time */

    rc = DosFindFirst((PSZ)path, (PHFILE)&handle, attr, (PVOID)&dir_buff, sizeof dir_buff, &searchcount, FF_LEVEL);

    if (rc != 0 && rc != ERROR_EAS_DIDNT_FIT)
    {
        HANDLE_OF(buf) = BAD_HANDLE;
        errno = ENOENT;
        return 1;
    }

    HANDLE_OF(buf) = handle;
    copydir(buf, &dir_buff);      /* copy in other fields */

    return 0;
}

static unsigned _dos_findnext( struct find_t *buf )
{
    APIRET rc;
    FF_BUFFER dir_buff;
    ULONG searchcount = 1;

    rc = DosFindNext(HANDLE_OF(buf), (PVOID)&dir_buff, sizeof dir_buff, &searchcount);

    if (rc != 0)
    {
        errno = ENOENT;
        return 1;
    }

    copydir( buf, &dir_buff );

    return 0;
}

unsigned _dos_findclose( struct find_t *buf )
{
    APIRET rc;

    if (HANDLE_OF(buf) != BAD_HANDLE)
    {
        rc = DosFindClose(HANDLE_OF(buf));

        if (rc != 0)
        {
            errno = ENOENT;
            return 1;
        }
    }

    return 0;
}

char *os_findfirst(struct _filefind *pff, const char *path, const char *mask)
{
    unsigned rc;
    char tmp[MYMAXPATH];

    strcpy(tmp, path);
    os_append_slash(tmp);
    strcat(tmp, mask);

    rc = _dos_findfirst(tmp, _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH, &pff->fileinfo);

    if (rc == 0)
    {
        return pff->fileinfo.name;
    }

    return NULL;
}

char *os_findnext(struct _filefind *pff)
{
    if (_dos_findnext(&pff->fileinfo) == 0)
    {
        return pff->fileinfo.name;
    }

    return NULL;
}

void os_findclose(struct _filefind *pff)
{
    _dos_findclose(&pff->fileinfo);
}

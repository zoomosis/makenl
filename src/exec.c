/** Warning! This is still PD, but had a minor change by Michael Karcher **/
/*
   EXEC.C: EXEC function with memory swap - Prepare parameters.

   Public domain software by

   Thomas Wagner
   Ferrari electronic GmbH
   Beusselstrasse 27
   D-1000 Berlin 21
   Germany

   BIXname: twagner
 */

/* $Id: exec.c,v 1.3 2004/07/13 04:42:54 ozzmosis Exp $ */

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <malloc.h>
#include <ctype.h>
#include <dos.h>
#include <io.h>
#include "fileutil.h"
#include "exec.h"
#include "os.h"
#include "spawn.h"

#define SWAP_FILENAME "$makenl.swp"

#ifndef __TURBOC__
#define stpcpy(d,s)     (strcpy (d, s), d + strlen (s))
#endif

/** Modification by Michael Karcher - added get_tempdir, find_executable and
    search into this source file **/
static void get_tempdir(char *tempdir)
{
    char *env;
    int len;
    char drive[MYMAXDRIVE];
    char ext[MYMAXEXT];
    struct SREGS sr;
    char name[MYMAXFILE];
    union REGS r;
    struct stat stbuf;
    char dir[MYMAXDIR];

    tempdir[0] = 0;
    if ((env = getenv("TMP")) == NULL && (env = getenv("TEMP")) == NULL)
        return;

    strcpy(tempdir, env);
    if ((len = strlen(tempdir)) == 0)
        return;

    if (tempdir[len - 1] == '/' || tempdir[len - 1] == '\\')
        tempdir[--len] = 0;

    myfnsplit(tempdir, drive, dir, name, ext);
    if (drive[0] != 0)
    {
        r.h.dl = toupper(drive[0]) - 'A' + 1;
        r.h.ah = 0x1C;
        intdosx(&r, &r, &sr);
        if (r.h.ah == 0xFF)
        {
            tempdir[0] = 0;
            return;
        }
    }
    if ((name[0] == 0 && dir[0] == 0) ||
        (stat(tempdir, &stbuf) == 0 && stbuf.st_mode & S_IWRITE
         && stbuf.st_mode & S_IEXEC))
    {
        tempdir[len] = '\\';
        tempdir[len + 1] = 0;
        return;
    }

}


static int find_executable(char *name)
{
    char *p = strchr(name, '\0');

    strcpy(p, ".COM");
    if (access(name, 0) == 0)
        return 1;

    strcpy(p, ".EXE");
    if (access(name, 0) == 0)
        return 1;

    strcpy(p, ".BAT");
    if (access(name, 0) == 0)
    {
        char tmp[MYMAXDIR];

        strcpy(tmp, getenv("COMSPEC"));
        strcat(tmp, " /c ");
        strcat(tmp, name);
        strcpy(name, tmp);
        return 1;
    }

    *p = '\0';
    return 0;
}


static int search(char *prgname)
{
    char *pathelement;
    int found;
    char drive[MYMAXDRIVE];
    char ext[MYMAXEXT];
    char name[MYMAXFILE];
    char dir[MYMAXDIR];
    char pathstr[256];

    if (prgname[0] == 0)
        strcpy(prgname, getenv("COMSPEC"));

    myfnsplit(prgname, drive, dir, name, ext);
    if (ext[0])
        found = access(prgname, 0) == 0;
    else
        found = find_executable(prgname);

    if (found || drive[0] != 0 || dir[0] != 0)
        return found;

    strcpy(pathstr, getenv("PATH"));
    pathelement = strtok(pathstr, ";");
    while (!found)
    {
        if (!pathelement)
            break;

        myfnmerge(prgname, drive, pathelement, name, ext);
        if (ext[0] != 0)
            found = access(prgname, 0) == 0;
        else
            found = find_executable(prgname);

        pathelement = strtok(NULL, ";");
    }

    return found;
}

int
do_exec(const char *exfn, const char *epars, int spwn, unsigned needed,
        char **envp)
{
    char swapfn[82];
    char execfn[82];
    unsigned avail;
    union REGS regs;
    unsigned envlen;
    int rc;
    int idx;
    char **env;
    char *ep, *envptr, *envbuf;
    int swapping;

    strcpy(execfn, exfn);

    if (!search(execfn))
        return RC_NOFILE;

    /* Now create a copy of the environment if the user wants it. */

    envlen = 0;
    envptr = envbuf = NULL;

    if (envp != NULL)
        for (env = envp; *env != NULL; env++)
            envlen += strlen(*env) + 1;

    if (envlen)
    {
        /* round up to paragraph, and alloc another paragraph leeway */
        envlen = (envlen + 32) & 0xfff0;
        envbuf = (char *)malloc(envlen);
        if (envbuf == NULL)
            return RC_ENVERR;

        /* align to paragraph */
        envptr = envbuf;
        if (FP_OFF(envptr) & 0x0f)
            envptr += 16 - (FP_OFF(envptr) & 0x0f);
        ep = envptr;

        for (env = envp; *env != NULL; env++)
        {
            ep = stpcpy(ep, *env) + 1;
        }
        *ep = 0;
    }

    if (!spwn)
        swapping = -1;
    else
    {
        /* Determine amount of free memory */

        regs.x.ax = 0x4800;
        regs.x.bx = 0xffff;
        intdos(&regs, &regs);
        avail = regs.x.bx;

        /* No swapping if available memory > needed */

        if (needed < avail)
            swapping = 0;
        else
        {
            /* Swapping necessary, use 'TMP' or 'TEMP' environment
               variable to determine swap file path if defined. */

            swapping = spwn;
            if (spwn & USE_FILE)
            {
                get_tempdir(swapfn);

                if (_osmajor >= 3)
                    swapping |= CREAT_TEMP;
                else
                {
                    strcat(swapfn, SWAP_FILENAME);
                    idx = strlen(swapfn) - 1;
                    while (access(swapfn, 0) == 0)
                    {
                        if (swapfn[idx] == 'Z')
                            idx--;
                        if (swapfn[idx] == '.')
                            idx--;
                        swapfn[idx]++;
                    }
                }
            }
        }
    }

    /* All set up, ready to go. */

    if (swapping > 0)
    {
        if (!envlen)
            swapping |= DONT_SWAP_ENV;

        rc = prep_swap(swapping, swapfn);
        if (rc < 0)
            return RC_PREPERR | -rc;
    }

    rc = do_spawn(swapping, execfn, epars, envlen, envptr);

    /* Free the environment buffer if it was allocated. */

    if (envlen)
        free(envbuf);

    return rc;
}

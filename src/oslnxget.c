/* $Id: oslnxget.c,v 1.2 2012/10/14 13:47:56 ozzmosis Exp $ */

char *os_fgets(char *buffer, size_t len, FILE * f)
{
    char *result;

#undef fgets
    result = fgets(buffer, len, f);
    /*
     * If we see a EOF at the beinning of a line, then make it look as
     * if we didn't read anything.
     */
    if (result != NULL && buffer[0] == ('Z' & 0x1F))
        return NULL;
    /* 
     * This should not happen, EOF should allways be at the begin of a line.
     */
    if (result != NULL && buffer[strlen(buffer) - 1] == ('Z' & 0x1F))
        buffer[strlen(buffer) - 1] = 0;
    return result;
}


/* $Id: oslnxget.c,v 1.2 2004/07/11 09:29:15 ozzmosis Exp $ */

char *os_fgets(char *buffer, size_t len, FILE * f)
{
    char *result;

#undef fgets
    result = fgets(buffer, len, f);
    if (result != NULL && buffer[strlen(buffer) - 1] == ('Z' & 0x1F))
        buffer[strlen(buffer) - 1] = 0;
    return result;
}
